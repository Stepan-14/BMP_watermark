cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

project(MyProject)
set(CMAKE_CXX_STANDARD 23)

add_subdirectory(lib)
add_subdirectory(bin)
