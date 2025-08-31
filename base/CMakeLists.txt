add_library(base OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log.cpp
)

target_include_directories(base PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)