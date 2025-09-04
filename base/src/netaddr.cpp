#include <arpa/inet.h>
#include "netaddr.h"
#include "log.h"

netaddr_t::netaddr_t(uint16_t port, const char* ipv4)
{
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
    if (ipv4 == nullptr) [[likely]] {
        _addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        int r = inet_pton(AF_INET, ipv4, &_addr.sin_addr.s_addr);
        if (r < 0) {
            LOG_ERROR("inet_pton : ip format error");
        }
    }
}

const std::string& netaddr_t::get_ip() const
{
    if (not _ip_cache.has_value()) [[unlikely]] {
        char ip[32] = {0};
        inet_ntop(AF_INET, &_addr.sin_addr.s_addr, ip, 32);
        _ip_cache = ip;
    }
    return _ip_cache.value();
}

uint16_t netaddr_t::get_port() const
{
    if (not _port_cache.has_value()) [[unlikely]] {
        _port_cache = htons(_addr.sin_port);
    }
    return _port_cache.value();
}