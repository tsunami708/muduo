#include <sys/eventfd.h>
#include "eventloop.h"
#include "channel.h"
#include "epoller.h"
#include "log.h"

thread_local eventloop_t* this_thread_eventloop = nullptr;

eventloop_t::eventloop_t()
    : _onwer_tid(this_thread_id), _epoller(new epoller_t(this)),
      _wakeup_fd(eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK))
{
    if (this_thread_eventloop) {
        LOG_FATAL("eventloop_t-ctor:multiple eventloop in a thread");
    }

    if (_wakeup_fd < 0) [[unlikely]] {
        LOG_FATAL("wakeup fd error");
    }

    this_thread_eventloop = this;
    _wakeup_channel = std::make_unique<channel_t>(this, _wakeup_fd);
    _wakeup_channel->set_read_cb([this] {
        uint64_t ch;
        ssize_t r = read(_wakeup_fd, &ch, 8);
        if (r != 8) [[unlikely]] {
            LOG_ERROR("handle_wakeup_channel_events:evenfd read fail");
        }
    });
    _wakeup_channel->enable_read();
}



eventloop_t::~eventloop_t()
{
    if (_looping_tag) {
        quit_loop();
    }
    this_thread_eventloop = nullptr;
}



void eventloop_t::check_thread()
{
    if (_onwer_tid != this_thread_id) {
        LOG_FATAL("eventloop_t-check_thread:illegal thread access");
    }
}



void eventloop_t::start_loop()
{
    check_thread();
    _quit_tag = false;
    _looping_tag = true;
    while (not _quit_tag) {
        _active_channels.clear();
        _epoller->wait(&_active_channels);
        for (channel_t* channel : _active_channels) {
            channel->handle_event();
        }
        handle_task();
    }
    _looping_tag = false;
}




void eventloop_t::update_channel(channel_t* channel)
{
    check_thread();
    _epoller->add_channel(channel);
}



void eventloop_t::run_task(wake_cb_t fn)
{
    if (is_io_thread() or _doing_wakeup_tag) {
        LOG_TRACE("run_task,directly call");
        fn();
    } else {
        LOG_TRACE("run_task,inqueue");
        {
            std::lock_guard<std::mutex> lock(_tasks_mtx);
            _tasks.emplace_back(fn);
        }
        wake();
    }
}



eventloop_t* eventloop_t::get_thread_eventloop() { return this_thread_eventloop; }



void eventloop_t::handle_task()
{
    std::vector<wake_cb_t> temp;
    {
        std::lock_guard<std::mutex> lock(_tasks_mtx);
        temp.swap(_tasks);
    }

    _doing_wakeup_tag = true;
    for (int i = 0; i < temp.size(); ++i) {
        temp[i]();
    }
    _doing_wakeup_tag = false;
}



void eventloop_t::wake()
{
    uint64_t ch = 1;
    ssize_t r = write(_wakeup_fd, &ch, 8);
    if (r != 8) [[unlikely]] {
        LOG_ERROR("wake:eventfd write fail");
    }
}