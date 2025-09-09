#ifdef LOG_TO_FILE
#include <fstream>
static std::ofstream file_to_log("record.log");
#endif
#include <cstring>
#include <format>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <signal.h>
#include "log.h"

std::string PTR(unsigned long long x)
{
    std::ostringstream ss;
    ss << "0x" << std::hex << x;
    return ss.str();
}

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

class signal_handler_t {
    inline static bool _handling = false;

public:
    signal_handler_t() { register_signal_handlers(); }

private:
    void register_signal_handlers()
    {
        signal(SIGINT, handle_signal);
        signal(SIGTERM, handle_signal);
        signal(SIGABRT, handle_signal);
        signal(SIGSEGV, handle_signal);
        signal(SIGPIPE, SIG_IGN);
    }

    static void handle_signal(int signum)
    {
        if (not _handling) {
            _handling = true;
            std::clog.flush();
            signal(signum, SIG_DFL);
            raise(signum);
        }
    }
};
static signal_handler_t sh;

log_t::log_t()
{
#ifdef LOG_TO_FILE
    std::clog.rdbuf(file_to_log.rdbuf());
#endif
}


void log_t::write_log(log_level_t level,
                      const char* file_path,
                      int line_number,
                      const std::string& message)
{
    std::clog << std::format("[{}][{}][{}][{}][{}]\n", STR_LEVEL(level), this_thread_id, message,
                             FILE_NAME(file_path), line_number);
    if (level == FATAL) [[unlikely]] {
        std::clog.flush();
        abort();
    }
}