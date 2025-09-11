set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "perfetto/sdk: ${PROJECT_SOURCE_DIR}/third_party/perfetto/sdk")
include_directories(${PROJECT_SOURCE_DIR}/third_party/perfetto/sdk)
add_executable(perfetto_demo perfetto_demo.cpp)
target_sources(perfetto_demo PRIVATE
    ${PROJECT_SOURCE_DIR}/third_party/perfetto/sdk/perfetto.cc
)
target_compile_definitions(perfetto_demo PRIVATE PERFETTO_TRACK_EVENT_ENABLED)