#include <cstdio>
#include "elthread.h"

void runInThread() { printf("runInThread(): pid = %d, tid = %d\n", getpid(), this_thread_id); }

int main()
{
    printf("main(): pid = %d, tid = %d\n", getpid(), this_thread_id);

    el_thread_t loopThread;
    eventloop_t* loop = loopThread.start();
    loop->run_inloop(runInThread);
    sleep(1);
    loop->run_after(2s, runInThread);
    sleep(3);
    loop->quit();

    printf("exit main().\n");
    return 0;
}