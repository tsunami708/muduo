#include <cassert>
#include <cstring>
#include <sys/eventfd.h>
#include "eventloop.h"
#include "timerqueue.h"
#include "epoller.h"
#include "channel.h"
#include "log.h"

thread_local eventloop_t* this_thread_eventloop = nullptr;

eventloop_t::eventloop_t()
    : _io_thread_id(this_thread_id), _wakefd(eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK)),
      _epoller(new epoller_t(this)), _timers(new timer_queue_t(this))
{
    if (_wakefd < 0) [[unlikely]]
        LOG_FATAL("eventfd fail"s);
    LOG_TRACE("Create an eventloop: "s + STR(this) + " ; onwer tid: "s + STR(_io_thread_id));
    if (this_thread_eventloop != nullptr)
        LOG_FATAL("Another eventloop exists"s);
    this_thread_eventloop = this;
    LOG_TRACE("Thread's eventloop: "s + STR(this_thread_eventloop));
    _waker = std::make_unique<channel_t>(this, _wakefd);
    _waker->set_read_cb([this] {
        uint64_t r;
        ssize_t n = read(_wakefd, &r, 8);
        if (n != 8) [[unlikely]]
            LOG_ERROR("read wakefd fail "s + strerror(errno));
    });
    _waker->enable_read();
}

eventloop_t::~eventloop_t()
{
    assert(not _looping);
    this_thread_eventloop = nullptr;
    LOG_TRACE("Destroy an eventloop"s);
}

void eventloop_t::start_loop()
{
    assert(not _looping);
    assert_in_io_thread();
    _looping = true;
    _quitted = false;
    LOG_TRACE("Start loop");
    while (not _quitted) {
        _epoller->wait(&_active_channels);
        for (channel_t* ch : _active_channels)
            ch->handle_event();
        do_task();
    }
    _looping = false;
    LOG_TRACE("End loop");
}

void eventloop_t::update_channel(channel_t* channel)
{
    assert(channel->get_onwer_loop() == this);
    assert_in_io_thread();
    _epoller->update_channel(channel);
}

void eventloop_t::remove_channel(channel_t* channel)
{
    assert(channel->get_onwer_loop() == this);
    assert_in_io_thread();
    _epoller->remove_channel(channel);
}

void eventloop_t::assert_in_io_thread()
{
    if (not is_in_io_thread())
        LOG_FATAL(" runs iothread-only operation"s);
}

void eventloop_t::run_after(duration_t delay, const timer_cb_t& func)
{
    LOG_TRACE("RUN_AFTER: "s + STR(delay.count()));
    _timers->add_timer(delay, func);
}
void eventloop_t::run_every(duration_t interval, const timer_cb_t& func)
{
    LOG_TRACE("RUN_EVERY: "s + STR(interval.count()));
    _timers->add_timer(duration_t::zero(), func, interval);
}

void eventloop_t::run_inloop(const user_fun_t& task)
{
    if (is_in_io_thread())
        task();
    else
        add_inloop(task);
}

void eventloop_t::add_inloop(const user_fun_t& task)
{
    LOG_TRACE("add a task to "s + STR(_io_thread_id) + "'s eventloop");
    {
        std::lock_guard<std::mutex> lock(_mtx);
        _pending_tasks.emplace_back(task);
    }
    if (not is_in_io_thread() or _doing_task)
        wake();
}

void eventloop_t::wake()
{
    uint64_t w = 1;
    ssize_t n = write(_wakefd, &w, 8);
    if (n != 8) [[unlikely]]
        LOG_ERROR("wake fail "s + strerror(errno));
}

void eventloop_t::do_task()
{
    _doing_task = true;
    std::vector<user_fun_t> t;
    {
        std::lock_guard<std::mutex> lock(_mtx);
        t.swap(_pending_tasks);
    }
    for (const user_fun_t& task : t)
        task();
    _doing_task = false;
}