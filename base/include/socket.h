#pragma once
#include <cstdint>
#include <string>
#include <netinet/in.h>
#include <optional>

#include "nocopy.h"
#include "fd.h"

// ipv4+port
class netaddr_t {
private:
    struct sockaddr_in _addr;
    mutable std::optional<std::string> _ip_cache;
    mutable std::optional<uint16_t> _port_cache;

public:
    netaddr_t() = default;
    netaddr_t(uint16_t port, const char* ipv4 = nullptr);
    inline const struct sockaddr_in& get_socketaddr() const { return _addr; }
    const std::string& get_ip() const;
    uint16_t get_port() const;
};



class socket_t : nocopy_t {
private:
    const fd_t _socket_fd;

public:
    static int create_socket();
    void bind_socket(const netaddr_t& local_addr);
    void listen_socket();
    socket_t* accept_request(netaddr_t* peer_addr);
    operator int() { return _socket_fd; }
    socket_t(int fd) : _socket_fd(fd) {}
};