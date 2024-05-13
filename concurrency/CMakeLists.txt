message(STATUS "------ concurrency start ------")

add_subdirectory(atm)
add_subdirectory(atomic_test)
add_subdirectory(basic)
add_subdirectory(lock_free)
add_subdirectory(sort)
add_subdirectory(thread_pool)
add_subdirectory(threadsafe_stack)

message(STATUS "------ concurrency end ------")