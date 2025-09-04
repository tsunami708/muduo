#pragma once
#include <memory>
#include <functional>
#include "nocopy.h"
#include "netaddr.h"
#include "channel.h"
#include "buffer.h"

class socket_t;

enum class conn_state_t { ESTABLISHING, ESTABLISHED, DESTROYING, DESTROYED };

class tcpconn_t : nocopy_t, public std::enable_shared_from_this<tcpconn_t> {
    using new_msg_cb_t = std::function<void(std::shared_ptr<tcpconn_t>)>;
    using conn_state_cb_t = std::function<void(std::shared_ptr<tcpconn_t>)>;

private:
    std::unique_ptr<socket_t> _conn_fd;
    const new_msg_cb_t& _new_msg_cb;       // when message arrives
    const conn_state_cb_t& _conn_state_cb; // when connection's state changes
    const netaddr_t _local_addr;
    const netaddr_t _peer_addr;
    conn_state_t _state = conn_state_t::ESTABLISHING;
    channel_t _channel;

    buffer_t _inbuf;

public:
    tcpconn_t(socket_t* conn_fd,
              eventloop_t* looper,
              const netaddr_t& local_addr,
              const netaddr_t& peer_addr,
              const new_msg_cb_t& mfn,
              const conn_state_cb_t& cfn);
    ~tcpconn_t();
    inline conn_state_t get_conn_state() { return _state; }
    inline void set_conn_state(conn_state_t state)
    {
        _state = state;
        if (_conn_state_cb) {
            _conn_state_cb(shared_from_this());
        }
    }
    inline buffer_t& get_inbuf() { return _inbuf; }

private:
    // these functions bind to channel
    void handle_read();
    // void handle_write();
    // void handle_error();
};