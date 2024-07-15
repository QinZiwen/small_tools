add_executable(gauss_newton gauss_newton.cpp)
add_dependencies(gauss_newton ceres-solver glog gflags eigen)
target_link_libraries(gauss_newton ceres)

add_executable(ceres_curve_fitting ceres_curve_fitting.cpp)
add_dependencies(ceres_curve_fitting ceres-solver glog gflags eigen)
target_link_libraries(ceres_curve_fitting ceres -lglog)
