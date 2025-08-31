cmake_minimum_required(VERSION 4.0)
project(muduo)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE DEBUG) #发布版本改为RELEASE


set(LOG_FILE OFF CACHE STRING "日志输出到record.log文件")
if(LOG_FILE)
    add_definitions(-DLOG_TO_FILE)
endif()


set(LOG_LEVEL "E" CACHE STRING "日志等级")
set(VALID_LOG_LEVELS T D I W E F)
if(NOT LOG_LEVEL IN_LIST VALID_LOG_LEVELS)
    message(FATAL_ERROR "无效的LOG_LEVEL:${LOG_LEVEL},必须是以下值之一::${VALID_LOG_LEVELS}")
endif()
if(LOG_LEVEL STREQUAL "T")
    add_definitions(-DENABLE_TRACE -DENABLE_DEBUG -DENABLE_INFO -DENABLEENABLE_WARN -DENABLE_ERROR -DENABLE_FATAL)
elseif(LOG_LEVEL STREQUAL "D")
    add_definitions(-DENABLE_DEBUG -DENABLE_INFO -DENABLE_WARN -DENABLE_ERROR -DENABLE_FATAL)
elseif(LOG_LEVEL STREQUAL "I")
    add_definitions(-DENABLE_INFO -DENABLE_WARN -DENABLE_ERROR -DENABLE_FATAL)
elseif(LOG_LEVEL STREQUAL "W")
    add_definitions(-DENABLE_WARN -DENABLE_ERROR -DENABLE_FATAL)
elseif(LOG_LEVEL STREQUAL "E")
    add_definitions(-DENABLE_ERROR -DENABLE_FATAL)
elseif(LOG_LEVEL STREQUAL "F")
    add_definitions(-DENABLE_FATAL)
endif()
message(STATUS "当前日志级别:${LOG_LEVEL}")


add_subdirectory(base)
add_subdirectory(net)
add_subdirectory(test)