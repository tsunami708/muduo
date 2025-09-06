#pragma once
#include <queue>
#include <chrono>
#include <functional>
#include "fd.h"
#include "channel.h"

class eventloop_t;

class timer_queue_t {
public:
    using clock_t = std::chrono::steady_clock;
    using time_point_t = clock_t::time_point;
    using duration_t = clock_t::duration;
    using timer_cb_t = std::function<void()>;

    struct timer_t {
        time_point_t expire;
        duration_t interval; // > 0 表示周期定时器
        timer_cb_t run_task;

        bool operator>(const timer_t& other) const { return expire > other.expire; }
    };
    timer_queue_t(eventloop_t* loop);
    ~timer_queue_t();
    void
    add_timer(duration_t delay, const timer_cb_t& func, duration_t interval = duration_t::zero());

    void handle_expired();

private:
    const fd_t _tfd;
    eventloop_t* _onwer_loop;
    channel_t _channel;
    std::priority_queue<timer_t, std::vector<timer_t>, std::greater<>> _timers;

    void update_timerfd();
};