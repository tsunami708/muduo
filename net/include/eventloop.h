#pragma once
#include <atomic>
#include <memory>
#include <vector>
#include "epoller.h"
#include "nocopy.h"



class channel_t;

class eventloop_t : nocopy_t {
private:
    const int _onwer_tid;

    std::atomic_bool _looping = false;
    std::atomic_bool _quit = true;

    std::unique_ptr<epoller_t> _epoller;
    std::vector<channel_t*> _active_channels; // output parameter for epoller
public:
    eventloop_t();
    void check_thread(); // some function can be only run by IO thread
    void start_loop();
    void update_channel(channel_t* channel);
    void quit_loop() { _quit = true; }

    static eventloop_t* get_thread_eventloop();
};