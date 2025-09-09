#pragma once
#include <vector>
#include "elthread.h"

class el_pool_t : nocopy_t {
private:
    std::vector<el_thread_t> _el_threads; // 每一个el_thread_t对象包含一个eventloop
    std::vector<eventloop_t*> _loopers;
    size_t _n;
    size_t _idx;
    eventloop_t _master;

public:
    el_pool_t(size_t n) : _el_threads(n), _n(n)
    {
        for (el_thread_t& item : _el_threads)
            _loopers.emplace_back(item.start());
    }
    inline eventloop_t* get_master() { return &_master; }
    // RR
    eventloop_t* fetch()
    {
        eventloop_t* res = _loopers[_idx];
        _idx = (_idx + 1) % _n;
        return res;
    }
};