#pragma once
#include <memory>
#include <functional>
#include <atomic>
#include "buffer.h"
#include "nocopy.h"
#include "socket.h"
#include "channel.h"


class eventloop_t;

class tcpconn_t : nocopy_t, public std::enable_shared_from_this<tcpconn_t> {
    using new_connection_cb_t = std::function<void(const std::shared_ptr<tcpconn_t>&)>;
    using close_connection_cb_t = std::function<void(const std::shared_ptr<tcpconn_t>&)>;
    using write_over_cb_t = std::function<void(const std::shared_ptr<tcpconn_t>&)>;
    using new_message_cb_t = std::function<void(const std::shared_ptr<tcpconn_t>&, buffer_t*)>;
    enum state_t { CONNECTING, CONNECTED, DISCONNECTED, DISCONNECTING };

private:
    eventloop_t* _looper;
    std::atomic<state_t> _state = CONNECTING;

    // pass by tcpserver
    const new_connection_cb_t& _conn_cb;
    const new_message_cb_t& _msg_cb;
    const write_over_cb_t& _wo_cb;
    const close_connection_cb_t _close_cb; // no reference
    //

    std::unique_ptr<socket_t> _socket;
    channel_t _channel;
    const netaddr_t _peer;

    buffer_t _input_buf;
    buffer_t _output_buf;

public:
    ~tcpconn_t();
    tcpconn_t(eventloop_t* loop,
              socket_t* psock,
              const netaddr_t& peer,
              const new_connection_cb_t& conn_cb,
              const new_message_cb_t& msg_cb,
              const close_connection_cb_t& close_cb,
              const write_over_cb_t& wo_cb);

    /// notify user
    void establish();
    void destroy();
    ///

    inline bool is_connected() { return _state == CONNECTED; }
    inline std::string get_peer()
    {
        return _peer.get_ip() + ":" + std::to_string(_peer.get_port());
    }
    inline int get_fd() { return *_socket; }
    inline eventloop_t* get_looper() { return _looper; }
    void send(std::string&& message);
    void shutdown(); // 告知对端不会再发送数据,用于优雅关闭连接

private:
    inline void set_state(state_t s) { _state = s; }

    void send_inloop(std::string&& message); // called by send
    void shutdown_inloop();                  // called by shutdown

    // bind to channel
    void handle_read();
    void handle_close();
    void handle_write();
    void handle_error();
};