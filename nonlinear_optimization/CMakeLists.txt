include(ExternalProject)

set(external_prefix ${CMAKE_BINARY_DIR}/external)
set(external_install_prefix ${CMAKE_BINARY_DIR}/external_install)

include_directories(${external_install_prefix}/include)
include_directories(${external_install_prefix}/include/eigen3)
link_directories(${external_install_prefix}/lib)

ExternalProject_Add(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG        3.4.0
    PREFIX ${external_prefix}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${external_install_prefix}
    BUILD_ALWAYS FALSE
    UPDATE_COMMAND ""
)

ExternalProject_Add(
    gflags
    GIT_REPOSITORY https://github.com/gflags/gflags.git
    GIT_TAG        v2.2.2
    PREFIX ${external_prefix}
    CMAKE_ARGS 
        -DCMAKE_INSTALL_PREFIX=${external_install_prefix}
        -DCMAKE_CXX_STANDARD=17
        -DBUILD_SHARED_LIBS=ON
    BUILD_ALWAYS FALSE
    UPDATE_COMMAND ""
)

ExternalProject_Add(
    glog
    GIT_REPOSITORY https://github.com/google/glog.git
    GIT_TAG v0.7.0
    PREFIX ${external_prefix}
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${external_install_prefix}
        -DCMAKE_CXX_STANDARD=17
        -DBUILD_SHARED_LIBS=ON
        -DWITH_GFLAGS=OFF
        -DWITH_GTEST=OFF
    BUILD_ALWAYS FALSE
    UPDATE_COMMAND ""
)
add_dependencies(glog gflags)
add_definitions(-DGLOG_USE_GLOG_EXPORT)

ExternalProject_Add(
    ceres-solver
    GIT_REPOSITORY https://github.com/ceres-solver/ceres-solver.git
    GIT_TAG 2.2.0
    PREFIX ${external_prefix}
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${external_install_prefix}
        -DCMAKE_CXX_STANDARD=17
        -DBUILD_SHARED_LIBS=ON
        -DBUILD_BENCHMARKS=OFF
        -DBUILD_DOCUMENTATION=OFF
        -DBUILD_EXAMPLES=OFF
        -DBUILD_TESTING=OFF
    BUILD_ALWAYS FALSE
    UPDATE_COMMAND ""
)
add_dependencies(ceres-solver glog eigen)

add_executable(gauss_newton gauss_newton.cpp)
add_dependencies(gauss_newton ceres-solver glog gflags eigen)
target_link_libraries(gauss_newton ceres)
