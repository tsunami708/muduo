#include <format>
#include <cstring>
#include <sys/socket.h>
#include "socket.h"
#include "log.h"

socket_t* socket_t::create_socket()
{
    int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (fd < 0) [[unlikely]] {
        LOG_ERROR(std::format("socket fd error:{}", strerror(errno)).c_str());
    }
    int opt = 1;
    int r = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
    if (r < 0) [[unlikely]] {
        LOG_ERROR(std::format("socket config error:{}", strerror(errno)).c_str());
    }
    return new socket_t(fd);
}



void socket_t::bind_socket(const netaddr_t& local_addr)
{
    int r = bind(_socket_fd, (const struct sockaddr*)&local_addr.get_socketaddr(),
                 sizeof local_addr.get_socketaddr());
    if (r < 0) [[unlikely]] {
        LOG_FATAL(std::format("bind_socket:{}", strerror(errno)).c_str());
    }
}



void socket_t::listen_socket()
{
    int r = listen(_socket_fd, SOMAXCONN);
    if (r < 0) [[unlikely]] {
        LOG_FATAL(std::format("listen_socket:{}", strerror(errno)).c_str());
    }
}



socket_t* socket_t::accept_request(netaddr_t* peer_addr)
{
    socklen_t len = sizeof peer_addr->get_socketaddr();
    int fd = accept(_socket_fd, (struct sockaddr*)&peer_addr->get_socketaddr(), &len);
    if (fd < 0) {
        LOG_WARN(std::format("accept_request fail:{}", strerror(errno)).c_str());
        return nullptr;
    }
    return new socket_t(fd);
}
