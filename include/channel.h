#pragma once
#include <functional>
#include "nocopy.h"

class eventloop_t;

/*
    每个Channel对象自始至终只属于一个EventLoop
    每个Channel对象自始至终只负责一个文件描述符（fd）的IO事件分发，但它并不拥有这个fd，也不会在析构的时候关闭这个fd
    Channel会把不同的IO事件分发为不同的回调，例如ReadCallback、WriteCallback
    用户不直接使用Channel，而会使用更上层的封装，如TcpConnection
    Channel的生命期由其ownerclass负责管理，它一般是其他class的直接或间接成员(timerqueue,tcpconnection)
*/
class channel_t : nocopy_t {
    using event_callback_t = std::function<void()>;
    static const int E_READ;
    static const int E_WRITE;

private:
    event_callback_t _read_cb;
    event_callback_t _write_cb;
    event_callback_t _error_cb;

    const int _fd;
    int _events;
    int _revents;
    eventloop_t* _onwer_loop;

public:
    channel_t(eventloop_t* onwer_loop, int fd) : _fd(fd), _onwer_loop(onwer_loop) {}
    ~channel_t();
    inline void set_read_cb(const event_callback_t& cb) { _read_cb = cb; }
    inline void set_write_cb(const event_callback_t& cb) { _write_cb = cb; }
    inline void set_error_cb(const event_callback_t& cb) { _error_cb = cb; }
    inline int get_fd() const { return _fd; }
    inline int get_events() const { return _events; }
    inline eventloop_t* get_onwer_loop() { return _onwer_loop; }
    inline void set_revents(int revent) { _revents = revent; }
    inline void enable_read()
    {
        _events |= E_READ;
        update();
    }
    inline void enable_write()
    {
        _events |= E_WRITE;
        update();
    }
    inline void disable_write()
    {
        _events &= ~E_WRITE;
        update();
    }
    inline void disable_all()
    {
        _events = 0;
        update();
    }
    void handle_event();

private:
    void update();
};