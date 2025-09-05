#pragma once

class nocopy_t {
protected:
    constexpr nocopy_t() = default;
    ~nocopy_t() = default;
    nocopy_t(const nocopy_t&) = delete;
    nocopy_t& operator=(const nocopy_t&) = delete;
};