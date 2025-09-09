#include "tcpserver.h"
#include "eventloop.h"
#include "tcpconn.h"
#include "buffer.h"
#include <stdio.h>

void onConnection(const std::shared_ptr<tcpconn_t>& conn)
{
    if (conn->is_connected()) {
        printf("onConnection(): new connection %s\n", conn->get_peer().c_str());
    } else {
        printf("onConnection(): connection [%s] is down\n", conn->get_peer().c_str());
    }
}

void onMessage(const std::shared_ptr<tcpconn_t>& conn, buffer_t* buf)
{
    printf("onMessage(): received %zd bytes from connection [%s]\n", buf->get_read_size(),
           conn->get_peer().c_str());
    std::string data = buf->read_all();
    printf("%s\n", data.c_str());
    conn->send(std::move(data));
}

void onWriteOver(const std::shared_ptr<tcpconn_t>& conn) { printf("data write over\n"); }

int main()
{
    eventloop_t loop;
    tcpserver_t server(&loop, {8080});

    server.set_conn_cb(onConnection);
    server.set_msg_cb(onMessage);
    server.set_wo_cb(onWriteOver);

    server.start();

    loop.start_loop();
    return 0;
}