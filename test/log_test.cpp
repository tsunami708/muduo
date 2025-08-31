#include <thread>

#include "log.h"

int main(){
    std::jthread t0([](){
        LOG_TRACE("message");
        LOG_DEBUG("message");
        LOG_INFO("message");
        LOG_WARN("message");
        LOG_ERROR("message");
    });

    std::jthread t2([](){
        LOG_TRACE("message");
        LOG_DEBUG("message");
        // LOG_FATAL("message_");
        LOG_INFO("message");
        LOG_WARN("message");
        LOG_ERROR("message");
    });

    LOG_TRACE("message");
    LOG_DEBUG("message");
    LOG_INFO("message");
    LOG_WARN("message");
    LOG_ERROR("message");

    return 0;
}