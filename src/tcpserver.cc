#include "tcpserver.h"
#include "eventloop.h"
#include "acceptor.h"
#include "tcpconn.h"
#include "log.h"
tcpserver_t::~tcpserver_t() {}
tcpserver_t::tcpserver_t(eventloop_t* loop, const netaddr_t& addr)
    : _loop(loop), _acceptor(new acceptor_t(loop, addr))
{
    LOG_TRACE("construct a tcpserver: "s + STR(this) + " looper is: " + STR(_loop));
    _acceptor->set_newconn_cb(
        [this](socket_t* connfd, const netaddr_t& paddr) { handle_connect(connfd, paddr); });
}

void tcpserver_t::start()
{
    if (not _started)
        _started = true;
    if (not _acceptor->is_listening())
        _acceptor->start_listen();
}

void tcpserver_t::handle_connect(socket_t* connfd, const netaddr_t& paddr)
{
    _loop->assert_in_io_thread();
    // connfd一定是非空指针
    _connections[*connfd] = std::make_shared<tcpconn_t>(
        _loop, connfd, paddr, _conn_cb, _msg_cb,
        [this](const std::shared_ptr<tcpconn_t> conn) { handle_close(conn); });
    _connections[*connfd]->establish();
}

void tcpserver_t::handle_close(const std::shared_ptr<tcpconn_t>& conn)
{
    _loop->assert_in_io_thread();
    LOG_TRACE("tcpserver_t::handle_close-"s + conn->get_peer());
    _connections.erase(conn->get_fd());
    _loop->add_inloop([conn] { conn->destroy(); }); // list must value-pass,no ref
}