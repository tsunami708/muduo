#include <sys/timerfd.h>
#include <cstring>
#include "eventloop.h"
#include "channel.h"

eventloop_t* g_loop;
void timeout()
{
    printf("Timeout!\n");
    g_loop->quit();
}

int main()
{
    eventloop_t loop;
    g_loop = &loop;

    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    channel_t channel(g_loop, timerfd);
    channel.set_read_cb(timeout);
    channel.enable_read();

    struct itimerspec howlong;
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);

    loop.start_loop();

    ::close(timerfd);
}