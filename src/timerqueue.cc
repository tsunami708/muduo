#include <cstring>
#include <sys/timerfd.h>
#include "timerqueue.h"
#include "log.h"

std::vector<std::unique_ptr<timer>> timerqueue_t::get_expired_timers()
{
    auto bound = _timer_set.upper_bound({timestamp::now(), nullptr});
    std::vector<std::unique_ptr<timer>> ret;

    for (auto iter = _timer_set.begin(); iter != bound; ++iter) {
        ret.emplace_back(std::move(const_cast<std::unique_ptr<timer>&>(iter->second)));
    }

    _timer_set.erase(_timer_set.begin(), bound);

    if (_timer_set.size() == 1)
        return ret;

    struct itimerspec ts;
    uint64_t timediff;
    bzero(&ts, sizeof ts);
    timediff = _timer_set.begin()->second->how_long_to_expire();
    ts.it_value.tv_sec = timediff / 1000000;
    ts.it_value.tv_nsec = timediff % 1000000 * 1000;
    int r = timerfd_settime(_timerfd, 0, &ts, nullptr);

    if (r == -1) [[unlikely]] {
        LOG_ERROR(STR(__func__) + strerror(errno));
    }

    return ret;
}

void timerqueue_t::add_timer(std::unique_ptr<timer> timer)
{
    if (_timer_set.begin()->first >= timer->get_expiration()) {
        struct itimerspec ts;
        uint64_t timediff;
        bzero(&ts, sizeof ts);

        timediff = timer->how_long_to_expire();
        ts.it_value.tv_sec = timediff / 1000000;
        ts.it_value.tv_nsec = timediff % 1000000 * 1000;

        int r = timerfd_settime(_timerfd, 0, &ts, nullptr);
        if (r == -1) [[unlikely]] {
            LOG_ERROR(STR(__func__) + strerror(errno));
        }
    }

    _timer_set.emplace(timer->get_expiration(), std::move(timer));
}

void timerqueue_t::handle_expired_timer()
{
    uint64_t flag;
    read(_timerfd, &flag, sizeof flag); // 清除标志,表示处理一次定时器事件

    std::vector<std::unique_ptr<timer>> expired_timers = get_expired_timers();

    for (std::unique_ptr<timer>& timer : expired_timers) {
        timer->run();
        if (timer->need_repeat()) {
            timer->delay();
            add_timer(std::move(timer));
        }
    }
}