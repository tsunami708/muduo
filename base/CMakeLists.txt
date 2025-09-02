add_library(base OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/netaddr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/socket.cpp
)

target_include_directories(base PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)