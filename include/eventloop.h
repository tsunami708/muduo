#pragma once
#include <atomic>
#include "nocopy.h"

extern thread_local int this_thread_id;

class eventloop_t : nocopy_t {
private:
    const int _io_thread_id;

    std::atomic_bool _looping = false;

public:
    eventloop_t();
    ~eventloop_t();

    void start_loop();

    inline bool is_in_io_thread() { return this_thread_id == _io_thread_id; }
    void assert_in_io_thread();
};
