include_directories(.)

add_executable(lock_free_stack lock_free_stack.cpp)
target_link_libraries(lock_free_stack pthread)
