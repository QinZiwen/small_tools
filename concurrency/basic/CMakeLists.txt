message(STATUS "------ basic start ------")

add_executable(thread_local thread_local.cpp)
target_link_libraries(thread_local pthread)

message(STATUS "------ basic end ------")