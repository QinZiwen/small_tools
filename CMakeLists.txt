# CMake Tools for Visual Studio Code documentation: https://github.com/microsoft/vscode-cmake-tools/blob/main/docs/README.md
# Using C++ and WSL in VS Code: https://code.visualstudio.com/docs/cpp/config-wsl
cmake_minimum_required(VERSION 3.15)
project(small_tools)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Debug")

# # 生成 compile_commands.json
# cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
# # 检查某个文件
# clang-tidy-18 src/foo.cpp -p=.
# small_tools/build$ clang-tidy-18 ../algorithm_practice/perfetto/perfetto_demo.cpp -p=.
# set(CMAKE_CXX_CLANG_TIDY "/usr/bin/clang-tidy-18;-p=${CMAKE_BINARY_DIR}")

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
elseif(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
endif()

add_subdirectory(algorithm_practice)
add_subdirectory(concurrency)
# add_subdirectory(nonlinear_optimization)
# add_subdirectory(cmake_practice)
add_subdirectory(msg_bus)
