#include <cassert>
#include <cstring>
#include <signal.h>
#include "tcpconn.h"
#include "eventloop.h"
#include "log.h"

struct signal_t {
    signal_t() { signal(SIGPIPE, SIG_IGN); }
};
signal_t global_sigpipe_ig;

tcpconn_t::tcpconn_t(eventloop_t* loop,
                     socket_t* psock,
                     const netaddr_t& peer,
                     const new_connection_cb_t& conn_cb,
                     const new_message_cb_t& msg_cb,
                     const close_connection_cb_t& close_cb,
                     const write_over_cb_t& wo_cb)
    : _looper(loop), _conn_cb(conn_cb), _msg_cb(msg_cb), _socket(psock), _channel(loop, *psock),
      _peer(peer), _close_cb(close_cb), _wo_cb(wo_cb)
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
    ssize_t n = _input_buf.read_fd(*_socket);
    if (n > 0)
        _msg_cb(shared_from_this(), &_input_buf);
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

void tcpconn_t::handle_error() { LOG_ERROR("Tcpconn::handle_error - "s + strerror(errno)); }

void tcpconn_t::handle_write()
{
    _looper->assert_in_io_thread();
    if (_channel.is_writing()) { // 写事件只在必要时处理
        ssize_t n = write(*_socket, _output_buf.peek(), _output_buf.get_read_size());
        if (n >= 0) {
            _output_buf.retrieve(n);
            if (_output_buf.get_read_size() == 0) { // 缓冲区发送完毕
                LOG_TRACE(get_peer() + " write over"s);
                _channel.disable_write();
                if (_wo_cb)
                    _wo_cb(shared_from_this());
                if (_state == DISCONNECTING)
                    shutdown_inloop();
            } else {
                LOG_TRACE(get_peer() + " is going to send more data"s);
                // 期待下一次写事件
            }
        } else {
            LOG_ERROR("handle_write error: "s + strerror(errno));
        }
    }
}
void tcpconn_t::send(std::string&& message)
{
    if (_state == CONNECTED) {
        _looper->run_inloop(
            [this, msg = std::move(message)]() mutable { send_inloop(std::move(msg)); });
    }
}
void tcpconn_t::send_inloop(std::string&& message)
{
    _looper->assert_in_io_thread();
    if (not _channel.is_writing() and _output_buf.get_read_size() == 0) {
        ssize_t n = write(*_socket, message.data(), message.length());
        if (n >= 0) {
            if (n < message.length()) { // 数据没有一次性发送完,剩余的先存入发送缓冲区
                LOG_INFO(get_peer() + " is goint to send more data"s);
                _output_buf.append(message.data() + n, message.length() - n);
                _channel.enable_write();
            }
        } else {
            if (errno != EWOULDBLOCK)
                LOG_ERROR("send_inloop error: "s + strerror(errno));
        }
    } else {
        // 缓冲区还有数据,不可以立刻发送(保证顺序)
        _output_buf.append(message.data(), message.length());
    }
}
void tcpconn_t::shutdown()
{
    if (_state == CONNECTED) {
        set_state(DISCONNECTING);
        _looper->run_inloop([this] { shutdown_inloop(); });
    }
}
void tcpconn_t::shutdown_inloop()
{
    _looper->assert_in_io_thread();
    if (not _channel.is_writing())
        ::shutdown(*_socket, SHUT_WR);
}