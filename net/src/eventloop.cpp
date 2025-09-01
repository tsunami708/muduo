#include "eventloop.h"
#include "channel.h"
#include "log.h"

thread_local eventloop_t* this_thread_eventloop = nullptr;

eventloop_t::eventloop_t() : _onwer_tid(this_thread_id), _epoller(new epoller_t(this))
{
    if (this_thread_eventloop) {
        LOG_FATAL("eventloop_t-ctor:multiple eventloop in a thread");
    }
    this_thread_eventloop = this;
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
    _quit = false;
    _looping = true;
    while (not _quit) {
        _active_channels.clear();
        _epoller->wait(&_active_channels);
        for (channel_t* channel : _active_channels) {
            channel->handle_event();
        }
    }
    _looping = false;
}




void eventloop_t::update_channel(channel_t* channel)
{
    check_thread();
    _epoller->add_channel(channel);
}




eventloop_t* eventloop_t::get_thread_eventloop() { return this_thread_eventloop; }