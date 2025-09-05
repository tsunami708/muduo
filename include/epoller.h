#pragma once
#include <vector>
#include <unordered_map>
#include "nocopy.h"
#include "fd.h"
/*
EPoller是EventLoop的间接成员，只供其ownerEventLoop在IO线程调用，因此无
须加锁。其生命期与EventLoop相等。EPoller并不拥有Channel，Channel在析构之
前必须自己unregister
*/
class eventloop_t;
class channel_t;
struct epoll_event;

class epoller_t : nocopy_t {
private:
    eventloop_t* _onwer_eventloop;
    const fd_t _epoll_fd;

    std::unordered_map<int, channel_t*> _channels;  // key is fd
    std::vector<struct epoll_event> _active_events; // output parameter for epoll_wait

public:
    epoller_t(eventloop_t* onwer_loop);
    ~epoller_t();
    void wait(std::vector<channel_t*>* active_channels);
    void update_channel(channel_t* channel);
};