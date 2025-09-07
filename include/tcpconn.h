#pragma once
#include <memory>
#include <string>
#include <functional>
#include "nocopy.h"
#include "socket.h"
#include "channel.h"
class eventloop_t;

class tcpconn_t : nocopy_t, public std::enable_shared_from_this<tcpconn_t> {
    using new_connection_cb_t = std::function<void(std::shared_ptr<tcpconn_t>&)>;
    using new_message_cb_t = std::function<void(std::shared_ptr<tcpconn_t>&, const char*, ssize_t)>;

private:
    eventloop_t* _looper;
    const std::string _name;
    const new_connection_cb_t& _conn_cb;
    const new_message_cb_t& _msg_cb;
    std::unique_ptr<socket_t> _socket;
    channel_t _channel;

public:
    tcpconn_t(eventloop_t* loop,
              socket_t* psock,
              const std::string name,
              const netaddr_t& local,
              const netaddr_t& peer,
              const new_connection_cb_t& conn_cb,
              const new_message_cb_t& msg_cb)
        : _name(name), _conn_cb(conn_cb), _msg_cb(msg_cb), _socket(psock), _channel(loop, *psock)
    {
    }
    void establish();
};