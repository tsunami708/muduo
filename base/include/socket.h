#pragma once
#include "nocopy.h"
#include "fd.h"
#include "netaddr.h"

class socket_t : nocopy_t {
private:
    const fd_t _socket_fd;

public:
    static socket_t* create_socket();
    void bind_socket(const netaddr_t& local_addr);
    void listen_socket();
    socket_t* accept_request(netaddr_t* peer_addr);
    operator int() { return _socket_fd; }

private:
    socket_t(int fd) : _socket_fd(fd) {}
};