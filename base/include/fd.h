#pragma once
#include "nocopy.h"

extern "C" int close(int __fd);

class fd_t : nocopy_t {
private:
    int _fd;

public:
    fd_t(int fd) : _fd(fd) {}
    ~fd_t() { close(_fd); }
    inline operator int() const { return _fd; }
};