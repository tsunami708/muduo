#include "acceptor.h"
#include "eventloop.h"
acceptor_t::~acceptor_t() {}
acceptor_t::acceptor_t(eventloop_t* loop, const netaddr_t& addr)
    : _onwer_loop(loop), _socketfd(socket_t::create_socket()), _channel(loop, _socketfd)
{
    _socketfd.bind_socket(addr);
    _channel.set_read_cb([this] { handle_connect(); });
    _channel.enable_read();
}

void acceptor_t::start_listen()
{
    if (not _listening) {
        _onwer_loop->assert_in_io_thread();
        _socketfd.listen_socket();
        _listening = true;
    }
}

void acceptor_t::handle_connect()
{
    _onwer_loop->assert_in_io_thread();
    netaddr_t peer_addr;
    socket_t* new_conn = _socketfd.accept_request(&peer_addr);
    if (new_conn)
        _newconn_cb(new_conn, peer_addr);
}