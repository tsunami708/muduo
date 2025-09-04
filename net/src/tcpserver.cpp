#include <format>
#include "tcpconn.h"
#include "acceptor.h"
#include "tcpserver.h"
#include "log.h"

tcpserver_t::~tcpserver_t() {}
tcpserver_t::tcpserver_t(eventloop_t* looper, const netaddr_t& listen_addr, const char* name)
    : _looper(looper), _local_addr(listen_addr),
      _underlying_acceptor(new acceptor_t(looper, listen_addr)), _name(name)
{
    _underlying_acceptor->set_new_conn_cb([this](socket_t* new_socket, const netaddr_t& peer_addr) {
        handle_new_conn(new_socket, peer_addr);
    });
}

void tcpserver_t::start() { _underlying_acceptor->start_listen(); }


void tcpserver_t::handle_new_conn(socket_t* new_socket, const netaddr_t& peer_addr)
{
    LOG_INFO(std::format("New Connection:[local:{}-{},peer:{}-{}]", _local_addr.get_ip(),
                         _local_addr.get_port(), peer_addr.get_ip(), peer_addr.get_port())
                 .c_str());
    _conn_pool[*new_socket] = std::shared_ptr<tcpconn_t>(
        new tcpconn_t(new_socket, _looper, _local_addr, peer_addr, _new_msg_cb, _conn_state_cb));
    _conn_pool[*new_socket]->set_conn_state(conn_state_t::ESTABLISHED);
}