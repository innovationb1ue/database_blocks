cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_C_COMPILER "/usr/bin/clang")
#set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
project(database_blocks)

#add_subdirectory(libs)

add_library(database_blocks mem_tree_impl.cpp mem_tree_impl.h db.h config.h db.cpp wal.h wal.cpp)

#target_link_libraries(database_blocks uuid_lib)
#include_directories(database_blocks libs)

# build tests ?
add_subdirectory(test)






