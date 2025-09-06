#pragma once
#include <atomic>
#include <memory>
#include <vector>
#include <functional>
#include <chrono>
#include "fd.h"
#include "nocopy.h"

using namespace std::chrono_literals;
extern thread_local int this_thread_id;

class channel_t;
class epoller_t;
class timer_queue_t;

class eventloop_t : nocopy_t {
    using user_fun_t = std::function<void()>;
    using timer_cb_t = std::function<void()>;
    using duration_t = std::chrono::steady_clock::duration;

private:
    const int _io_thread_id;
    const fd_t _wakefd;
    std::atomic_bool _looping = false;
    std::atomic_bool _quitted = true;
    std::atomic_bool _doing_task = false;
    std::unique_ptr<epoller_t> _epoller;
    std::unique_ptr<timer_queue_t> _timers;
    std::unique_ptr<channel_t> _waker;
    std::vector<channel_t*> _active_channels;
    std::vector<user_fun_t> _pending_tasks;
    std::mutex _mtx; // protect pending_tasks
public:
    eventloop_t();
    ~eventloop_t();

    void start_loop();
    inline void quit()
    {
        _quitted = true;
        if (not is_in_io_thread())
            wake();
    }
    void update_channel(channel_t* channel);
    inline bool is_in_io_thread() { return this_thread_id == _io_thread_id; }

    void run_after(duration_t delay, const timer_cb_t& func);
    void run_every(duration_t interval, const timer_cb_t& func);

    /*
    EventLoop 有一个非常有用的功能：在它的 IO 线程内执行某个用户任务回调，
    即 EventLoop::runInLoop(const Functor& cb)
    如果用户在当前 IO 线程调用这个函数，回调会同步进行；如果用户在其他线程调用
    runInLoop()，cb 会被加入队列，IO 线程会被唤醒来调用这个 Functor
    */
    void run_inloop(const user_fun_t& task);

    /*
    明确哪些成员函数是线程安全的，可以跨线程调用；哪些成员函数只能在某个特定线程调用（主要是IO线程）
    */
    void assert_in_io_thread();

private:
    void add_inloop(const user_fun_t& task);
    void wake();
    void do_task();
};
