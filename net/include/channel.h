#pragma once
#include <cstdint>
#include <functional>
#include "nocopy.h"



class eventloop_t;

class channel_t : nocopy_t {
    using read_cb_t = std::function<void()>;

private:
    static const uint32_t EREAD;

    eventloop_t* const _onwer_eventloop;

    const int _fd;
    uint32_t _events;
    uint32_t _revents;

    read_cb_t _read_event_cb;

public:
    channel_t(eventloop_t* onwer_loop, int fd) : _onwer_eventloop(onwer_loop), _fd(fd) {}

    void set_read_cb(read_cb_t fn) { _read_event_cb = fn; }
    void set_revents(uint32_t revents) { _revents = revents; }
    void enable_read()
    {
        _events |= EREAD;
        update();
    }
    void handle_event()
    {
        if (_revents & EREAD and _read_event_cb) {
            _read_event_cb();
        }
    }

    inline int get_fd() { return _fd; }
    inline uint32_t get_events() { return _events; }

private:
    void update(); // 通知_onwer_eventloop更新此channel
};