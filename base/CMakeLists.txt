add_library(base OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/socket.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/buffer.cc
)

target_include_directories(base PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)