# CMake Tools for Visual Studio Code documentation: https://github.com/microsoft/vscode-cmake-tools/blob/main/docs/README.md
# Using C++ and WSL in VS Code: https://code.visualstudio.com/docs/cpp/config-wsl
cmake_minimum_required(VERSION 3.15)
project(small_tools)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE "Debug")

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
elseif(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
endif()

add_subdirectory(algorithm_practice)
add_subdirectory(concurrency)
