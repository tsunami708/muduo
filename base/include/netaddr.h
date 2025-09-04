#pragma once
#include <cstdint>
#include <string>
#include <netinet/in.h>
#include <optional>
// ipv4+port
class netaddr_t {
private:
    struct sockaddr_in _addr;
    mutable std::optional<std::string> _ip_cache;
    mutable std::optional<uint16_t> _port_cache;

public:
    netaddr_t(uint16_t port, const char* ipv4 = nullptr);
    netaddr_t() = default;
    inline const struct sockaddr_in& get_socketaddr() const { return _addr; }
    const std::string& get_ip() const;
    uint16_t get_port() const;
};