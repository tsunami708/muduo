#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "socket.h"
#include "log.h"

netaddr_t::netaddr_t(uint16_t port, const char* ipv4)
{
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
    if (ipv4 == nullptr) [[likely]] {
        _addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        int r = inet_pton(AF_INET, ipv4, &_addr.sin_addr.s_addr);
        if (r < 0)
            LOG_ERROR("inet_pton : ip format error"s);
    }
}

const std::string& netaddr_t::get_ip() const
{
    if (not _ip_cache.has_value()) [[unlikely]] {
        LOG_DEBUG("ip cache");
        char ip[32] = {0};
        inet_ntop(AF_INET, &_addr.sin_addr.s_addr, ip, 32);
        _ip_cache = ip;
    }
    LOG_DEBUG(_ip_cache.value());
    return _ip_cache.value();
}

uint16_t netaddr_t::get_port() const
{
    if (not _port_cache.has_value()) [[unlikely]] {
        LOG_DEBUG("port cache");
        _port_cache = ntohs(_addr.sin_port);
    }
    LOG_DEBUG(STR(_port_cache.value()));
    return _port_cache.value();
}


int socket_t::create_socket()
{
    int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (fd < 0) [[unlikely]]
        LOG_ERROR("socket create fail: "s + strerror(errno));

    int opt = 1;
    int r = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
    if (r < 0) [[unlikely]]
        LOG_ERROR("socket opt fail: "s + strerror(errno));
    return fd;
}



void socket_t::bind_socket(const netaddr_t& local_addr)
{
    int r = bind(_socket_fd, (const struct sockaddr*)&local_addr.get_socketaddr(),
                 sizeof local_addr.get_socketaddr());
    if (r < 0) [[unlikely]]
        LOG_FATAL("bind fail: "s + strerror(errno));
}



void socket_t::listen_socket()
{
    int r = listen(_socket_fd, SOMAXCONN);
    if (r < 0) [[unlikely]]
        LOG_FATAL("listen fail: "s + strerror(errno));
}



socket_t* socket_t::accept_request(netaddr_t* peer_addr)
{
    socklen_t len = sizeof peer_addr->get_socketaddr();
    int fd = accept(_socket_fd, (struct sockaddr*)&peer_addr->get_socketaddr(), &len);
    if (fd < 0) {
        LOG_WARN("accept fail: "s + strerror(errno));
        return nullptr;
    }
    return new socket_t(fd);
}
