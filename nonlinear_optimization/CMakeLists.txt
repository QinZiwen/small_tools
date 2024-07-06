include(FetchContent)
FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG        3.4.0
)
FetchContent_MakeAvailable(eigen)

add_executable(gauss_newton gauss_newton.cpp)
target_link_libraries(gauss_newton eigen)
