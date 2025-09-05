#pragma once
#include <string>
#include "nocopy.h"

using namespace std::string_literals;
enum log_level_t { TRACE, DEBUG, WARN, INFO, ERROR, FATAL };


std::string PTR(unsigned long long x);
inline std::string STR(auto x)
{
    if constexpr (std::is_pointer_v<decltype(x)>) {
        return PTR((unsigned long long)x);
    } else {
        return std::to_string(x);
    }
}

class log_t : nocopy_t {
public:
    log_t();
    void write_log(log_level_t level,
                   const char* file_path,
                   int line_number,
                   const std::string& message);
};


extern thread_local log_t this_thread_log;
extern thread_local int this_thread_id;


#ifdef ENABLE_TRACE
#define LOG_TRACE(message) this_thread_log.write_log(TRACE, __FILE__, __LINE__, message)
#else
#define LOG_TRACE(message)
#endif


#ifdef ENABLE_DEBUG
#define LOG_DEBUG(message) this_thread_log.write_log(DEBUG, __FILE__, __LINE__, message)
#else
#define LOG_DEBUG(message)
#endif


#ifdef ENABLE_INFO
#define LOG_INFO(message) this_thread_log.write_log(INFO, __FILE__, __LINE__, message)
#else
#define LOG_INFO(message)
#endif


#ifdef ENABLE_WARN
#define LOG_WARN(message) this_thread_log.write_log(WARN, __FILE__, __LINE__, message)
#else
#define LOG_WARN(message)
#endif


#ifdef ENABLE_ERROR
#define LOG_ERROR(message) this_thread_log.write_log(ERROR, __FILE__, __LINE__, message)
#else
#define LOG_ERROR(message)
#endif


#ifdef ENABLE_FATAL
#define LOG_FATAL(message) this_thread_log.write_log(FATAL, __FILE__, __LINE__, message)
#else
#define LOG_FATAL(message)
#endif
