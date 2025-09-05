#include <thread>

#include "include/eventloop.h"


using namespace std;

int main()
{
    eventloop_t el;

    jthread t([&] { el.start_loop(); });

    el.start_loop();
    return 0;
}