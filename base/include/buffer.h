#pragma once
#include <cstdint>
#include <vector>
#include <string>


class buffer_t {
private:
    inline static const uint32_t RESERVE_ZONE_END = 8;
    inline static const uint32_t INIT_SIZE = 2048;

    uint32_t _read_ptr = RESERVE_ZONE_END;
    uint32_t _write_ptr = RESERVE_ZONE_END;

    std::vector<char> _buffer;

public:
    /*                           r            w
        -----------------------------------------------------
        \            \           \            \             \
        \RESERVE_ZONE\   free    \            \             \
        \            \           \            \             \
        -----------------------------------------------------
    */
    buffer_t() : _buffer(INIT_SIZE) {}

    inline std::string read_all()
    {
        std::string res(_buffer.data() + _read_ptr, get_read_size());
        _read_ptr = _write_ptr = RESERVE_ZONE_END;
        return res;
    }
    inline std::string read_part(uint32_t len)
    {
        if (len > get_read_size()) {
            return read_all();
        }
        std::string res(_buffer.data() + _read_ptr, len);
        _read_ptr += len;
        return res;
    }
    inline uint32_t get_read_size() { return _write_ptr - _read_ptr; }
    inline uint32_t get_write_size() { return _buffer.size() - _write_ptr + 1; }
    inline uint32_t get_free_size() { return _read_ptr - RESERVE_ZONE_END; }
    ssize_t read_fd(int fd);

private:
    void move_read_data();

    inline void make_space(int delta) { _buffer.resize(_buffer.size() + delta); }
    inline void append(char buf[], int size)
    {
        for (int i = 0; i < size; ++i) {
            _buffer[_write_ptr++] = buf[i];
        }
    }
};
