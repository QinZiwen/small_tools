# CMake Tools for Visual Studio Code documentation: https://github.com/microsoft/vscode-cmake-tools/blob/main/docs/README.md
# Using C++ and WSL in VS Code: https://code.visualstudio.com/docs/cpp/config-wsl
cmake_minimum_required(VERSION 3.15)
project(small_tools)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(concurrency)
