add_library(net STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/channel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/eventloop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/epoller.cpp
)

target_include_directories(net PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(net PUBLIC base)