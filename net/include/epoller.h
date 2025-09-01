#pragma once
#include <unordered_map>
#include <vector>
#include <sys/epoll.h>
#include "nocopy.h"



class eventloop_t;
class channel_t;


class epoller_t : nocopy_t {
private:
    eventloop_t* const _onwer_eventloop;
    const int _epoll_fd;

    std::unordered_map<int, channel_t*> _channels;      // key is fd
    std::vector<struct epoll_event> _active_events{10}; // output parameter for epoll_wait

public:
    epoller_t(eventloop_t* onwer_loop);
    ~epoller_t();
    void wait(std::vector<channel_t*>* active_channels);
    void add_channel(channel_t* channel); // add or update
};