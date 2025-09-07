#include "tcpserver.h"
#include "eventloop.h"
#include "tcpconn.h"
#include <stdio.h>

void onConnection(const std::shared_ptr<tcpconn_t>& conn)
{
    printf("%p\n", conn.get());
    if (conn->is_connected()) {
        printf("onConnection(): new connection %s\n", conn->get_peer().c_str());
    } else {
        printf("onConnection(): connection [%s] is down\n", conn->get_peer().c_str());
    }
}

void onMessage(const std::shared_ptr<tcpconn_t>& conn, const char* data, ssize_t len)
{
    printf("%p\n", conn.get());
    printf("onMessage(): received %zd bytes from connection [%s]\n", len, conn->get_peer().c_str());
}

int main()
{
    printf("main(): pid = %d\n", getpid());

    eventloop_t loop;

    tcpserver_t server(&loop, {8080});
    server.set_conn_cb(onConnection);
    server.set_msg_cb(onMessage);
    server.start();

    loop.start_loop();
    return 0;
}