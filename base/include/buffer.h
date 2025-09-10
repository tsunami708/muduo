#pragma once
#include <vector>
#include <string>


class buffer_t {
private:
    inline static const size_t RESERVE_ZONE_END = 8;
    inline static const size_t INIT_SIZE = 2048;

    size_t _read_ptr = RESERVE_ZONE_END;
    size_t _write_ptr = RESERVE_ZONE_END;

    std::vector<char> _buffer;

public:
    /// +-------------------+------------------+------------------+
    /// | prependable bytes |  readable bytes  |  writable bytes  |
    /// |                   |     (CONTENT)    |                  |
    /// +-------------------+------------------+------------------+
    /// |                   |                  |                  |
    /// 0      <=      readerIndex   <=   writerIndex    <=     size
    buffer_t() : _buffer(INIT_SIZE) {}

    ssize_t read_fd(int fd);
    inline std::string read_all()
    {
        std::string res(_buffer.data() + _read_ptr, get_read_size());
        _read_ptr = _write_ptr = RESERVE_ZONE_END;
        return res;
    }
    inline std::string read_part(size_t len)
    {
        if (len > get_read_size()) {
            return read_all();
        }
        std::string res(_buffer.data() + _read_ptr, len);
        _read_ptr += len;
        return res;
    }


    // 返回可读位置
    inline char* peek() { return _buffer.data() + _read_ptr; }

    // 配合tcpconn_t::send使用,移动读指针
    inline void retrieve(size_t len) { _read_ptr += len; }

    inline size_t get_read_size() { return _write_ptr - _read_ptr; }
    inline size_t get_write_size() { return _buffer.size() - _write_ptr + 1; }
    inline size_t get_free_size() { return _read_ptr - RESERVE_ZONE_END; }

    inline void append(char buf[], size_t size)
    {
        for (int i = 0; i < size; ++i) {
            _buffer[_write_ptr++] = buf[i];
        }
    }

private:
    // 将可读区域的数据整体向前挪动
    void move_read_data();

    inline void make_space(int delta) { _buffer.resize(_buffer.size() + delta); }
};
