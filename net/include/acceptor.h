#pragma once
#include <memory>
#include <functional>
#include "channel.h"
#include "socket.h"
#include "log.h"
#include "netaddr.h"

class eventloop_t;

class acceptor_t {
    using new_conn_cb_t = std::function<void(const netaddr_t&)>;

private:
    std::unique_ptr<socket_t> _underlying_socket; // for listening
    channel_t _underlying_channel;

    new_conn_cb_t _new_conn_cb;

public:
    acceptor_t(eventloop_t* onwer_loop)
        : _underlying_socket(socket_t::create_socket()),
          _underlying_channel(onwer_loop, *_underlying_socket)
    {
        _underlying_channel.set_read_cb([this] { handle_read(); });
    }
    inline void set_new_conn_cb(new_conn_cb_t fn) { _new_conn_cb = fn; }
    inline void start_listen(const netaddr_t& addr)
    {
        _underlying_socket->bind_socket(addr);
        _underlying_socket->listen_socket();
        _underlying_channel.enable_read();
    }

private:
    void handle_read() // for channel's read cb
    {
        netaddr_t peer_addr;
        socket_t* com_socket = _underlying_socket->accept_request(&peer_addr);
        if (com_socket) {
            LOG_INFO("a new link");
            _new_conn_cb(peer_addr);
        }
    }
};