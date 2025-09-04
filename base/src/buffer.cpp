#include <cstring>
#include <format>
#include <sys/uio.h>
#include <algorithm>
#include "buffer.h"
#include "log.h"


ssize_t buffer_t::read_fd(int fd)
{
    char extrabuf[65536]{};
    uint32_t ws = get_write_size(); // writeable_size
    struct iovec iv[2];
    iv[0].iov_base = _buffer.data() + _write_ptr;
    iv[0].iov_len = ws;
    iv[1].iov_base = extrabuf;
    iv[1].iov_len = sizeof extrabuf;
    ssize_t n = readv(fd, iv, 2);
    if (n >= 0) {
        if (long diff = n - (ssize_t)ws; diff > 0) {
            _write_ptr += ws;
            if (diff > get_free_size()) {
                make_space(diff << 1);
            } else {
                move_read_data();
            }
            append(extrabuf, diff);
        } else {
            _write_ptr += n;
        }
    } else {
        LOG_ERROR(std::format("readv error: {}", strerror(errno)).c_str());
    }
    return n;
}


void buffer_t::move_read_data()
{
    uint32_t backsize = get_free_size();
    auto iter = _buffer.begin();
    std::copy(iter + _read_ptr, iter + _write_ptr, iter + RESERVE_ZONE_END);
    _read_ptr = RESERVE_ZONE_END;
    _write_ptr -= backsize;
}
