#pragma once
#include <atomic>
#include <memory>
#include <vector>
#include <functional>
#include <chrono>
#include "nocopy.h"

using namespace std::chrono_literals;
extern thread_local int this_thread_id;

class channel_t;
class epoller_t;
class timer_queue_t;

class eventloop_t : nocopy_t {
    using timer_cb_t = std::function<void()>;
    using duration_t = std::chrono::steady_clock::duration;

private:
    const int _io_thread_id;
    std::atomic_bool _looping = false;
    std::atomic_bool _quitted = true;
    std::unique_ptr<epoller_t> _epoller;
    std::unique_ptr<timer_queue_t> _timers;
    std::vector<channel_t*> _active_channels;

public:
    eventloop_t();
    ~eventloop_t();

    void start_loop();
    inline void quit() { _quitted = true; }
    void update_channel(channel_t* channel);
    inline bool is_in_io_thread() { return this_thread_id == _io_thread_id; }

    void run_after(duration_t delay, const timer_cb_t& func);
    void run_every(duration_t interval, const timer_cb_t& func);

    /*
    明确哪些成员函数是线程安全的，可以跨线程调用；哪些成员函数只能在某个特定线程调用（主要是IO线程）
    */
    void assert_in_io_thread();
};
