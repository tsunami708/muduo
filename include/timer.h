#pragma once
#include <cstdint>
#include <functional>
#include <sys/time.h>

inline uint64_t operator"" _s(unsigned long long scalar) { return scalar * 1000000; }
inline uint64_t operator"" _ms(unsigned long long scalar) { return scalar * 1000; }
inline uint64_t operator"" _us(unsigned long long scalar) { return scalar; }

class timestamp {
public:
    static inline uint64_t now()
    { // 返回当前时间戳,us
        gettimeofday(&tv, nullptr);
        return (uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec;
    }
    static inline uint64_t future(uint64_t scalar) { return now() + scalar; }

private:
    inline static struct timeval tv;
};



class timer {
public:
    using timer_callback_t = std::function<void()>;

    timer(uint64_t expiration, const timer_callback_t& fn, uint64_t interval = 0)
        : _expiration(expiration), _fn(fn), _interval(interval)
    {
    }

    inline uint64_t get_expiration() { return _expiration; }

    inline uint64_t how_long_to_expire()
    {
        int64_t diff = _expiration - timestamp::now();
        return diff > 100 ? diff : 100;
    }

    inline bool need_repeat() { return _interval > 0; }

    inline void run() { _fn(); }

    inline void delay() { _expiration = timestamp::future(_interval); }

private:
    uint64_t _expiration;     // 到期时间戳,us
    const uint64_t _interval; // 周期,0表示一次性
    timer_callback_t _fn;
};