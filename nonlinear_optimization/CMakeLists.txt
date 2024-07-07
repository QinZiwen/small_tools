add_executable(gauss_newton gauss_newton.cpp)
add_dependencies(gauss_newton ceres-solver glog gflags eigen)
target_link_libraries(gauss_newton ceres)
