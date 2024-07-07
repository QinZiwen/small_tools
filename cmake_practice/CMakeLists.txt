add_executable(ceres_test ceres_test.cpp)
add_dependencies(ceres_test ceres-solver)
target_link_libraries(ceres_test ceres -lglog -lgflags)