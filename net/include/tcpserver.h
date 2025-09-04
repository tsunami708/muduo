#pragma once
#include <memory>
#include <unordered_map>
#include <functional>
#include "nocopy.h"
#include "netaddr.h"

class acceptor_t;
class tcpconn_t;
class eventloop_t;
class socket_t;

class tcpserver_t : nocopy_t {
    // for tcpconnection_t
    using new_msg_cb_t = std::function<void(std::shared_ptr<tcpconn_t>)>;
    using conn_state_cb_t = std::function<void(std::shared_ptr<tcpconn_t>)>;

    // for acceptor_t
    using new_conn_cb_t = std::function<void(socket_t*, const netaddr_t&)>;

private:
    /*
        Acceptor receive request--->Return a Connection--->Tcpserver manages all connections
    */
    std::unique_ptr<acceptor_t> _underlying_acceptor;
    std::unordered_map<int, std::shared_ptr<tcpconn_t> > _conn_pool; // key is socketfd

    new_msg_cb_t _new_msg_cb;
    conn_state_cb_t _conn_state_cb;

    const char* _name;
    const netaddr_t& _local_addr;
    eventloop_t* const _looper;

public:
    tcpserver_t(eventloop_t* looper, const netaddr_t& listen_addr, const char* name);
    ~tcpserver_t();
    inline void set_conn_state_cb(conn_state_cb_t fn) { _conn_state_cb = fn; }
    inline void set_new_msg_cb(new_msg_cb_t fn) { _new_msg_cb = fn; }
    void start();

private:
    void handle_new_conn(socket_t* new_socket, const netaddr_t& peer_addr);
};