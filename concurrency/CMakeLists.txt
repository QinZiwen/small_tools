message(STATUS "------ concurrency start ------")

add_subdirectory(atm)
add_subdirectory(basic)
add_subdirectory(sort)
add_subdirectory(threadsafe_stack)

message(STATUS "------ concurrency end ------")