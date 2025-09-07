#pragma once
#include <memory>
#include <functional>
#include <atomic>
#include "nocopy.h"
#include "socket.h"
#include "channel.h"


class eventloop_t;

class tcpconn_t : nocopy_t, public std::enable_shared_from_this<tcpconn_t> {
    using new_connection_cb_t = std::function<void(const std::shared_ptr<tcpconn_t>&)>;
    using new_message_cb_t
        = std::function<void(const std::shared_ptr<tcpconn_t>&, const char*, ssize_t)>;
    enum state_t { CONNECTING, CONNECTED };

private:
    eventloop_t* _looper;
    std::atomic<state_t> _state = CONNECTING;
    const new_connection_cb_t& _conn_cb;
    const new_message_cb_t& _msg_cb;
    std::unique_ptr<socket_t> _socket;
    channel_t _channel;
    const netaddr_t _peer;

public:
    ~tcpconn_t();
    tcpconn_t(eventloop_t* loop,
              socket_t* psock,
              const netaddr_t& peer,
              const new_connection_cb_t& conn_cb,
              const new_message_cb_t& msg_cb);
    void establish();
    inline bool is_connected() { return _state == CONNECTED; }
    inline std::string get_peer()
    {
        return _peer.get_ip() + ":" + std::to_string(_peer.get_port());
    }

private:
    inline void set_state(state_t s) { _state = s; }

    // bind to channel
    void handle_read();
};