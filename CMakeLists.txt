cmake_minimum_required(VERSION 3.24)
project(database_blocks)

set(CMAKE_CXX_STANDARD 17)

add_library(database_blocks mem_tree_impl.cpp)


add_subdirectory(test)
# import library test




