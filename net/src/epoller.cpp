#include <cstring>
#include <format>
#include <unistd.h>
#include <sys/epoll.h>
#include "epoller.h"
#include "channel.h"
#include "log.h"
epoller_t::~epoller_t() {}
epoller_t::epoller_t(eventloop_t* onwer_loop)
    : _onwer_eventloop(onwer_loop), _epoll_fd(epoll_create1(EPOLL_CLOEXEC))
{
    if (_epoll_fd < 0) [[unlikely]] {
        LOG_FATAL(std::format("epoller_t-ctor:{}", strerror(errno)).c_str());
    }
    _active_events.resize(10);
}



#define TIMEOUT -1
void epoller_t::wait(std::vector<channel_t*>* active_channels)
{
    int num = epoll_wait(_epoll_fd, _active_events.data(), _active_events.size(), TIMEOUT);
    if (num > 0) {
        // LOG_TRACE("epoller_t::wait events happened");
        for (int i = 0; i < num; ++i) {
            const struct epoll_event& event = _active_events[i];
            channel_t* channel = _channels[event.data.fd];
            channel->set_revents(event.events);
            active_channels->emplace_back(channel);
        }
    } else if (num == 0) {
        LOG_INFO("epoller_t::wait timeout");
    } else {
        LOG_ERROR(std::format("epoller_t::wait {}", strerror(errno)).c_str());
    }
}



void epoller_t::add_channel(channel_t* channel)
{
    int fd = channel->get_fd();
    uint32_t events = channel->get_events();

    struct epoll_event e;
    e.events = events;
    e.data.fd = fd;

    int r;
    if (not _channels.count(fd)) [[likely]] {
        r = epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &e);
    } else {
        r = epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &e);
    }
    if (r < 0) [[unlikely]] {
        LOG_ERROR(std::format("epoller_t - add_channel:{}", strerror(errno)).c_str());
        return;
    }

    _channels[fd] = channel;

    if (_channels.size() > _active_events.size()) {
        _active_events.resize(_active_events.size() * 2);
    }
}