message(STATUS "------ basic start ------")

add_executable(thread_local thread_local.cpp)
target_link_libraries(thread_local pthread)

add_executable(protected_test protected_test.cpp)

add_executable(unique_lock_test unique_lock_test.cpp)
target_link_libraries(unique_lock_test pthread)

message(STATUS "------ basic end ------")