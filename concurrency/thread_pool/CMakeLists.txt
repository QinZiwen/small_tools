add_executable(thread_pool_v1 thread_pool_v1.cpp)
target_link_libraries(thread_pool_v1 pthread)

add_executable(thread_pool_v2 thread_pool_v2.cpp)
target_link_libraries(thread_pool_v2 pthread)

add_executable(thread_pool_v3 thread_pool_v3.cpp)
target_link_libraries(thread_pool_v3 pthread)

add_executable(thread_pool_v4 thread_pool_v4.cpp)
target_link_libraries(thread_pool_v4 pthread -latomic)
