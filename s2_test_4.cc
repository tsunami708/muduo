// copied from muduo/net/tests/TimerQueue_unittest.cc

#include "eventloop.h"

#include <functional>
#include <cstdio>

using namespace std;

int cnt = 0;
eventloop_t* g_loop;

void print(const char* msg)
{
    printf("MSG: %s\n", msg);
    if (++cnt == 20) {
        g_loop->quit();
    }
}

int main()
{
    eventloop_t loop;
    g_loop = &loop;

    loop.run_after(1us, bind(print, "once1"));
    loop.run_after(2us, bind(print, "once2"));
    loop.run_after(3us, bind(print, "once3"));
    loop.run_after(3us, bind(print, "once3"));
    loop.run_every(4us, bind(print, "every4"));
    loop.run_every(5us, bind(print, "every5"));

    loop.start_loop();
    return 0;
}
