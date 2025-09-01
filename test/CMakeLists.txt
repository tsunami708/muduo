add_executable(log_test ${CMAKE_CURRENT_SOURCE_DIR}/log_test.cpp)
target_link_libraries(log_test PRIVATE base)

add_executable(eventloop_test ${CMAKE_CURRENT_SOURCE_DIR}/eventloop_test.cpp)
target_link_libraries(eventloop_test PRIVATE net)

add_executable(task_test ${CMAKE_CURRENT_SOURCE_DIR}/task_test.cpp)
target_link_libraries(task_test PRIVATE net)
