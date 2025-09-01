#include <thread>
#include <iostream>
#include "eventloop.h"

using namespace std;

int main()
{
    eventloop_t* eventloop = new eventloop_t;
    eventloop->run_task([] { cout << "主线程的任务\n"; });
    jthread th0([eventloop] {
        this_thread::sleep_for(4s);
        eventloop->run_task([] {
            sleep(2);
            cout << "子线程0的任务\n";
        });
    });
    jthread th1([eventloop] {
        this_thread::sleep_for(10s);
        eventloop->run_task([eventloop] {
            cout << "子线程1的任务\n";
            eventloop->run_task([] { cout << "xxxxxxxx\n"; });
        });
    });
    jthread th2([eventloop] {
        this_thread::sleep_for(1s);
        eventloop->run_task([eventloop] {
            cout << "子线程2的任务\n";
            eventloop->run_task([eventloop] {
                cout << "yyyyyyyy\n";
                eventloop->run_task([] { cout << "zzzzzzzzzzz\n"; });
            });
        });
    });
    jthread th3([eventloop] {
        this_thread::sleep_for(3s);
        eventloop->run_task([] { cout << "子线程3的任务\n"; });
    });
    eventloop->start_loop();
    delete eventloop;
    return 0;
}
