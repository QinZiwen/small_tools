include(FetchContent)
include(ExternalProject)

fetchContent_declare(  
    gflags  
    GIT_REPOSITORY https://github.com/google/gflags.git
    GIT_TAG        3.2.3 
    GIT_SHALLOW    TRUE  
)
FetchContent_MakeAvailable(gflags)

fetchContent_declare(  
    glog  
    GIT_REPOSITORY https://github.com/google/glog.git 
    GIT_TAG        v0.7.0 
    GIT_SHALLOW    TRUE  
)
FetchContent_MakeAvailable(eigen)

ExternalProject_Add(
    ceres-solver
    GIT_REPOSITORY https://github.com/ceres-solver/ceres-solver.git
    GIT_TAG 2.2.0
    PREFIX ${CMAKE_BINARY_DIR}/external_project
    INSTALL_DIR ${CMAKE_BINARY_DIR}/external_project/install
    CMAKE_ARGS -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF -DBUILD_BENCHMARKS=OFF -DBUILD_SHARED_LIBS=ON
)

add_executable(ceres_test ceres_test.cpp)
add_dependencies(ceres_test ceres-solver)
target_link_libraries(ceres_test ceres)