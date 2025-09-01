#include <sys/epoll.h>
#include "channel.h"
#include "eventloop.h"

const uint32_t channel_t::EREAD = EPOLLIN | EPOLLPRI | EPOLLRDBAND;

void channel_t::update() { _onwer_eventloop->update_channel(this); }