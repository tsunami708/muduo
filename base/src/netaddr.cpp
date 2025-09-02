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