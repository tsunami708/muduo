#include <cassert>
#include "tcpconn.h"
#include "eventloop.h"
#include "log.h"

tcpconn_t::tcpconn_t(eventloop_t* loop,
                     socket_t* psock,
                     const netaddr_t& peer,
                     const new_connection_cb_t& conn_cb,
                     const new_message_cb_t& msg_cb)
    : _looper(loop), _conn_cb(conn_cb), _msg_cb(msg_cb), _socket(psock), _channel(loop, *psock),
      _peer(peer)
{
    LOG_INFO("TcpConnection is created:"s);
}
tcpconn_t::~tcpconn_t() { LOG_INFO("TcpConnection is destroyed"s); }

void tcpconn_t::establish()
{
    assert(_state == CONNECTING);
    _looper->assert_in_io_thread();
    set_state(CONNECTED);
    _channel.set_read_cb([this] { handle_read(); });
    _channel.enable_read();
    _conn_cb(shared_from_this());
}

void tcpconn_t::handle_read()
{
    // TEST
    char buf[2048]{};
    ssize_t n = read(*_socket, buf, sizeof buf);
    _msg_cb(shared_from_this(), buf, n);
}