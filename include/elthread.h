#pragma once
#include <thread>
#include <semaphore>
#include "nocopy.h"
#include "eventloop.h"

class el_thread_t : nocopy_t {
private:
    std::binary_semaphore _sem{0};
    std::jthread _th;
    eventloop_t* _looper;

public:
    eventloop_t* start()
    {
        _th = std::jthread([this] {
            eventloop_t looper;
            _looper = &looper;
            _sem.release();
            looper.start_loop();
        });
        _sem.acquire();
        return _looper;
    }
};