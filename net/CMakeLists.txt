add_library(net STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/eventloop.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/channel.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/epoller.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/timerqueue.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/acceptor.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tcpserver.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tcpconn.cc
)

target_include_directories(net PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(net PUBLIC base)