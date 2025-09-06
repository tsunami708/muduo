#include <cassert>
#include "eventloop.h"
#include "timerqueue.h"
#include "epoller.h"
#include "channel.h"
#include "log.h"

thread_local eventloop_t* this_thread_eventloop = nullptr;

eventloop_t::eventloop_t()
    : _io_thread_id(this_thread_id), _epoller(new epoller_t(this)), _timers(new timer_queue_t(this))
{
    LOG_TRACE("Create an eventloop: "s + STR(this) + " ; onwer tid: "s + STR(_io_thread_id));
    if (this_thread_eventloop != nullptr)
        LOG_FATAL("Another eventloop exists"s);
    this_thread_eventloop = this;
    LOG_TRACE("Thread's eventloop: "s + STR(this_thread_eventloop));
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