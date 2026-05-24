cmake_minimum_required(VERSION 4.2)
project(set9)

set(CMAKE_CXX_STANDARD 20)

add_executable(benchmark
        src/main.cpp
        src/string_generator.cpp
        src/string_sort_algorithms.cpp
        src/string_sort_tester.cpp)
target_include_directories(benchmark PRIVATE include)
