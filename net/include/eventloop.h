#pragma once
#include <atomic>
#include <functional>
#include <memory>
#include <vector>
#include "nocopy.h"
#include "fd.h"



class epoller_t;
class channel_t;

extern thread_local int this_thread_id;

class eventloop_t : nocopy_t {
    using wake_cb_t = std::function<void()>;

private:
    const int _onwer_tid;

    std::atomic_bool _looping_tag = false;
    std::atomic_bool _quit_tag = true;

    const fd_t _wakeup_fd;
    std::atomic_bool _doing_wakeup_tag = false;
    std::vector<wake_cb_t> _tasks;
    std::mutex _tasks_mtx;
    std::unique_ptr<channel_t> _wakeup_channel;

    std::unique_ptr<epoller_t> _epoller;
    std::vector<channel_t*> _active_channels; // output parameter for epoller
public:
    eventloop_t();
    ~eventloop_t();
    void check_thread(); // some function can be only run by IO thread
    inline bool is_io_thread() { return this_thread_id == _onwer_tid; }
    void start_loop();
    void update_channel(channel_t* channel);
    void quit_loop() // Multithreaded calls
    {
        _quit_tag = true;
        if (not is_io_thread()) {
            wake();
        }
    }
    void run_task(wake_cb_t); // Multithreaded calls
    static eventloop_t* get_thread_eventloop();

private:
    void handle_task();
    void wake();
};