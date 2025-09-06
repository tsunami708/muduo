#pragma once
#include <set>
#include <memory>
#include <vector>

#include "nocopy.h"
#include "channel.h"
#include "timer.h"
#include "fd.h"


class eventloop_t;

class timerqueue_t : nocopy_t {
public:
    timerqueue_t(int fd, eventloop_t* loop) : _timerfd(fd), _onwer_loop(loop), _channel(loop, fd)
    {
        _timer_set.emplace(UINT64_MAX, nullptr);
        _channel.set_read_cb([this] { handle_expired_timer(); });
        _channel.enable_read();
    }

    /*加入定时器到队列中*/
    inline void add_timer(const timer::timer_callback_t& cb, uint64_t delay, uint64_t interval)
    {
        add_timer(std::make_unique<timer>(timestamp::future(delay), cb, interval));
    }
    void add_timer(std::unique_ptr<timer> timer);

    /*
        从_timer_set中取出已经调用的定时器后交给handle_expired_timer,逐个调用到期定时器的run方法
    */
    std::vector<std::unique_ptr<timer>> get_expired_timers();
    void handle_expired_timer();

private:
    std::set<std::pair<uint64_t, std::unique_ptr<timer>>> _timer_set; // 按到期时间升序

    const fd_t _timerfd;
    eventloop_t* _onwer_loop;
    channel_t _channel; // 内嵌Channel结构(非指针),将TimeQueue视作一个特殊的Channel
};