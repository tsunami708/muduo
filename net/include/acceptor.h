#pragma once
#include <functional>
#include "nocopy.h"
#include "channel.h"
#include "socket.h"

class eventloop_t;

class acceptor_t : nocopy_t {
    using new_conn_cb_t = std::function<void(socket_t*, const netaddr_t&)>;

private:
    eventloop_t* _onwer_loop;
    socket_t _socketfd;
    channel_t _channel;
    new_conn_cb_t _newconn_cb; // pass by tcpserver
    bool _listening = false;

public:
    inline void set_newconn_cb(const new_conn_cb_t& cb) { _newconn_cb = cb; }
    acceptor_t(eventloop_t* loop, const netaddr_t& addr);
    ~acceptor_t();
    void start_listen();
    inline bool is_listening() { return _listening; }

private:
    void handle_connect();
};