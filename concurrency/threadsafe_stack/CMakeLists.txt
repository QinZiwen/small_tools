include_directories(.)

add_executable(threadsafe_queue threadsafe_queue.cpp)
target_link_libraries(threadsafe_queue pthread)

add_executable(threadsafe_stack threadsafe_stack.cpp)
target_link_libraries(threadsafe_stack pthread)
