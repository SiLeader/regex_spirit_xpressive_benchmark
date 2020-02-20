cmake_minimum_required(VERSION 3.15)
project(regex_bench)

set(CMAKE_CXX_STANDARD 14)

add_executable(regex_bench main.cpp xpressive.cpp regex.cpp spirit.cpp types.hpp utils_for_regex.hpp)