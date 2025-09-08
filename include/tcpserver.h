#pragma once
#include <memory>
#include <unordered_map>
#include <functional>
#include "socket.h"
#include "nocopy.h"

class eventloop_t;
class acceptor_t;
class tcpconn_t;
class buffer_t;

class tcpserver_t : nocopy_t {
    using new_connection_cb_t = std::function<void(const std::shared_ptr<tcpconn_t>&)>;
    using new_message_cb_t = std::function<void(const std::shared_ptr<tcpconn_t>&, buffer_t*)>;

private:
    bool _started = false;
    eventloop_t* _loop;
    std::unique_ptr<acceptor_t> _acceptor;
    std::unordered_map<int, std::shared_ptr<tcpconn_t>> _connections;

    // pass to tcpconn
    new_connection_cb_t _conn_cb; // 连接状态发生变化时回调
    new_message_cb_t _msg_cb;     // 对端消息到达时回调

public:
    tcpserver_t(eventloop_t* loop, const netaddr_t& addr);
    ~tcpserver_t();

    // It's harmless to call it multiple times.
    // Thread safe.
    void start();

    // not thread safe
    inline void set_conn_cb(const new_connection_cb_t& cb) { _conn_cb = cb; }
    inline void set_msg_cb(const new_message_cb_t& cb) { _msg_cb = cb; }

private:
    // for acceptor . Not thread safe, but in loop
    void handle_connect(socket_t* connfd, const netaddr_t& paddr);

    // for connection
    void handle_close(const std::shared_ptr<tcpconn_t>& conn);
};