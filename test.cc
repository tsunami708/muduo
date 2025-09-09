#include "tcpserver.h"
#include "tcpconn.h"
#include <cstdio>


using tcpconn_ptr = const std::shared_ptr<tcpconn_t>;

void on_connection(tcpconn_ptr& conn)
{
    printf("连接建立,连接名:%s,分配到的looper:%p\n", conn->get_peer().c_str(), conn->get_looper());
}
void on_written(tcpconn_ptr& conn)
{
    printf("连接:%s数据已经发送完毕,即将下线\n", conn->get_peer().c_str());
}
void on_message(tcpconn_ptr& conn, buffer_t* buf)
{
    auto data = buf->read_all();
    printf("服务器收到来自连接:%s的消息,将发回%s\n", conn->get_peer().c_str(), data.c_str());
    conn->send(std::move(data));
}

int main()
{
    tcpserver_t server({8080, "127.0.0.1"}, 10);
    server.set_conn_cb(on_connection);
    server.set_wo_cb(on_written);
    server.set_msg_cb(on_message);
    server.start();
    return 0;
}
