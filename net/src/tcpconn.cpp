#include <cstring>
#include "tcpconn.h"
#include "socket.h"
#include "log.h"
tcpconn_t::~tcpconn_t() {}
tcpconn_t::tcpconn_t(socket_t* conn_fd,
                     eventloop_t* looper,
                     const netaddr_t& local_addr,
                     const netaddr_t& peer_addr,
                     const new_msg_cb_t& mfn,
                     const conn_state_cb_t& cfn)
    : _conn_fd(conn_fd), _channel(looper, *conn_fd), _local_addr(local_addr), _peer_addr(peer_addr),
      _new_msg_cb(mfn), _conn_state_cb(cfn)
{
    _channel.set_read_cb([this] { handle_read(); });
    _channel.enable_read();
}

void tcpconn_t::handle_read()
{
    int fd = *_conn_fd;

    ssize_t n = _inbuf.read_fd(fd);
    if (n > 0) {
        LOG_TRACE("Recv message");
        _new_msg_cb(shared_from_this());
    } else if (n == 0) {
        // close connection
        // LOG_TRACE("connection closed");
        // tell looper remove self's channel,then remove from conn_pool
    } else {
        // handle_error();
    }
}
// void tcpconn_t::handle_write() {}
// void tcpconn_t::handle_error()
// {
//     LOG_ERROR(std::format("connection read error: {}", strerror(errno)).c_str());
// }