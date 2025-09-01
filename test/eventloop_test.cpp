// #include <thread>
#include <unistd.h>
#include "channel.h"
#include "eventloop.h"
#include "log.h"
int main()
{
    // thread test

    // eventloop_t* test_eventloop = new eventloop_t;
    // std::jthread th([test_eventloop]() {
    //     // test_eventloop->start_loop();
    //     channel_t* test_channel = new channel_t(test_eventloop, 0);
    //     test_channel->enable_read();
    // });
    // test_eventloop->start_loop();
    // delete test_eventloop;

    // loop test
    int pipefd1[2] = {0};
    pipe(pipefd1);
    pid_t id = fork();
    if (id > 0) {
        close(pipefd1[1]);
        int pipefd2[2] = {0};
        pipe(pipefd2);
        pid_t id = fork();
        if (id > 0) {
            close(pipefd2[1]);
            eventloop_t test_eventloop;
            channel_t *channel1, *channel2;

            channel1 = new channel_t(&test_eventloop, pipefd1[0]);
            channel1->set_read_cb([pipefd1]() {
                LOG_TRACE("收到子进程1号的消息");
                char buf[5] = {0};
                read(pipefd1[0], buf, 5);
            });
            channel1->enable_read();


            channel2 = new channel_t(&test_eventloop, pipefd2[0]);
            channel2->set_read_cb([pipefd2]() {
                LOG_TRACE("收到子进程2号的消息");
                char buf[5] = {0};
                read(pipefd2[0], buf, 5);
            });
            channel2->enable_read();

            test_eventloop.start_loop();

            delete channel1;
            delete channel2;

        } else if (id == 0) {
            close(pipefd2[0]);
            for (int i = 0; i < 5; ++i) {
                write(pipefd2[1], "1111", 5);
                sleep(1);
            }
        } else {
            LOG_ERROR("fork error");
        }

    } else if (id == 0) {
        close(pipefd1[0]);
        for (int i = 0; i < 5; ++i) {
            write(pipefd1[1], "1111", 5);
            sleep(1);
        }
    } else {
        LOG_ERROR("fork error");
    }

    return 0;
}