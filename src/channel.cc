#include <sys/epoll.h>
#include "channel.h"
#include "eventloop.h"

const int channel_t::E_READ = EPOLLIN | EPOLLPRI | EPOLLHUP;
const int channel_t::E_WRITE = EPOLLOUT;

void channel_t::update() { _onwer_loop->update_channel(this); }

/*
Channel::handle_event()是Channel的核心，它由EventLoop::start_loop()调用，根据revents的值分别调用不同的用户回调
*/
void channel_t::handle_event()
{
    if (_revents & EPOLLERR and _error_cb)
        _error_cb();
    if (_revents & E_WRITE and _write_cb)
        _write_cb();
    if (_revents & E_READ and _read_cb)
        _read_cb();
}