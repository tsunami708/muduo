#include <sys/timerfd.h>
#include <unistd.h>
#include <cstring>
#include "timerqueue.h"
#include "eventloop.h"
#include "log.h"


timer_queue_t::~timer_queue_t() {}
timer_queue_t::timer_queue_t(eventloop_t* loop)
    : _tfd(timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)), _onwer_loop(loop),
      _channel(loop, _tfd)
{
    if (_tfd == -1)
        LOG_FATAL(STR(__func__) + strerror(errno));
    _channel.set_read_cb([this] { handle_expired(); });
    _channel.enable_read();
}


void timer_queue_t::add_timer(duration_t delay, const timer_cb_t& func, duration_t interval)
{
    _onwer_loop->assert_in_io_thread();
    timer_t t{clock_t::now() + delay, interval, std::move(func)};
    _timers.push(std::move(t));
    update_timerfd();
}

void timer_queue_t::handle_expired()
{
    _onwer_loop->assert_in_io_thread();
    uint64_t expirations;
    if (read(_tfd, &expirations, sizeof(expirations)) != sizeof(expirations))
        return; // 没有真正超时

    auto now = clock_t::now();
    while (not _timers.empty() and _timers.top().expire <= now) {
        timer_t t = std::move(_timers.top());
        _timers.pop();

        t.run_task();

        if (t.interval > duration_t::zero()) {
            t.expire = now + t.interval;
            _timers.push(std::move(t));
        }
    }
    update_timerfd();
}


void timer_queue_t::update_timerfd()
{
    if (_timers.empty()) {
        struct itimerspec new_value {};
        timerfd_settime(_tfd, 0, &new_value, nullptr); // disarm
        return;
    }

    auto now = clock_t::now();
    auto next = _timers.top().expire;
    auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(next - now);

    if (diff.count() < 0)
        diff = std::chrono::nanoseconds(1000);

    struct itimerspec new_value {};
    new_value.it_value.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(diff).count();
    new_value.it_value.tv_nsec = (diff % std::chrono::seconds(1)).count();

    int r = timerfd_settime(_tfd, 0, &new_value, nullptr);
    if (r < 0)
        LOG_ERROR("timerfd_settime"s + strerror(errno));
}
