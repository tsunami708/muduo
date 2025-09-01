#ifdef LOG_TO_FILE
#include <fstream>
static std::ofstream file_to_log("record.log");
#endif
#include <cstring>
#include <format>
#include <iostream>
#include <unistd.h>
#include "log.h"



constexpr const char* STR_LEVEL(log_level_t level)
{
    switch (level) {
    case TRACE:
        return "TRACE";
    case DEBUG:
        return "DEBUG";
    case INFO:
        return "INFO";
    case WARN:
        return "WARN";
    case ERROR:
        return "ERROR";
    case FATAL:
        return "FATAL";
    default:
        return "UNKNOW";
    }
}



inline const char* FILE_NAME(const char* file_path) { return strrchr(file_path, '/') + 1; }



thread_local log_t this_thread_log;
thread_local int this_thread_id = gettid();



log_t::log_t()
{
#ifdef LOG_TO_FILE
    std::clog.rdbuf(file_to_log.rdbuf());
#endif
}



void log_t::write_log(log_level_t level,
                      const char* file_path,
                      int line_number,
                      const char* message)
{
    std::clog << std::format("[{}][{}][{}][{}][{}]\n", STR_LEVEL(level), this_thread_id, message,
                             FILE_NAME(file_path), line_number);
    if (level == FATAL) [[unlikely]] {
        std::clog.flush();
        abort();
    }
}