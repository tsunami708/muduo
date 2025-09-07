#include <cassert>
#include <cstring>
#include "tcpconn.h"
#include "eventloop.h"
#include "log.h"

tcpconn_t::tcpconn_t(eventloop_t* loop,
                     socket_t* psock,
                     const netaddr_t& peer,
                     const new_connection_cb_t& conn_cb,
                     const new_message_cb_t& msg_cb,
                     const close_connection_cb_t& close_cb)
    : _looper(loop), _conn_cb(conn_cb), _msg_cb(msg_cb), _socket(psock), _channel(loop, *psock),
      _peer(peer), _close_cb(close_cb)
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
    _channel.set_write_cb([this] { handle_write(); });
    _channel.set_error_cb([this] { handle_error(); });
    _channel.enable_read();
    _conn_cb(shared_from_this());
}

void tcpconn_t::destroy()
{
    assert(_state == CONNECTED);
    _looper->assert_in_io_thread();
    LOG_TRACE("tcpconn_t::destroy");
    set_state(DISCONNECTED);
    _channel.disable_all();
    _conn_cb(shared_from_this());
    _looper->remove_channel(&_channel);
}

void tcpconn_t::handle_read()
{
    // TEST
    char buf[2048]{};
    ssize_t n = read(*_socket, buf, sizeof buf);
    if (n > 0)
        _msg_cb(shared_from_this(), buf, n);
    else if (n == 0)
        handle_close();
    else
        handle_error();
}

void tcpconn_t::handle_close()
{
    assert(_state == CONNECTED);
    _looper->assert_in_io_thread();
    LOG_TRACE("tcpconn_t::handle_close-"s + get_peer());
    _channel.disable_all();
    _close_cb(shared_from_this());
}

void tcpconn_t::handle_write() {}

void tcpconn_t::handle_error() { LOG_ERROR("Tcpconn::handle_error - "s + strerror(errno)); }