#pragma once
#include <cstdint>
#include <netinet/in.h>

// ipv4+port
class netaddr_t {
private:
    struct sockaddr_in _addr;

public:
    netaddr_t(uint16_t port, const char* ipv4 = nullptr);
    netaddr_t() = default;
    inline const struct sockaddr_in& get_socketaddr() const { return _addr; }
};