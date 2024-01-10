message(STATUS "------ basic start ------")

add_executable(thread_local thread_local.cpp)
target_link_libraries(thread_local pthread)

add_executable(protected_test protected_test.cpp)

message(STATUS "------ basic end ------")