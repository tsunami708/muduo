#include "tcpserver.h"
#include "eventloop.h"
#include "acceptor.h"
#include "tcpconn.h"
#include "elpool.h"
#include "log.h"
tcpserver_t::~tcpserver_t() {}
tcpserver_t::tcpserver_t(const netaddr_t& addr, size_t n)
    : _loop_pool(new el_pool_t(n)), _loop(_loop_pool->get_master()),
      _acceptor(new acceptor_t(_loop, addr))
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
    _loop->start_loop();
}

void tcpserver_t::handle_connect(socket_t* connfd, const netaddr_t& paddr)
{
    _loop->assert_in_io_thread();
    // connfd一定是非空指针
    eventloop_t* looper = _loop_pool->fetch();
    _connections[*connfd] = std::make_shared<tcpconn_t>(
        looper, connfd, paddr, _conn_cb, _msg_cb,
        [this](const std::shared_ptr<tcpconn_t> conn) { handle_close(conn); }, _wo_cb);
    looper->run_inloop([conn = _connections[*connfd]] { conn->establish(); });
}

void tcpserver_t::handle_close(const std::shared_ptr<tcpconn_t>& conn)
{
    LOG_TRACE("tcpserver_t::handle_close-"s + conn->get_peer());
    _loop->run_inloop([this, conn] { _connections.erase(conn->get_fd()); });
    // 先在主_looper中删除连接池中的conn,再交给conn的looper进行收尾工作
    eventloop_t* looper = conn->get_looper();
    looper->add_inloop([conn] { conn->destroy(); }); // list must value-pass,no ref
}