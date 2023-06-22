cmake_minimum_required(VERSION 3.24)
project(database_blocks)

set(CMAKE_CXX_STANDARD 17)

add_library(database_blocks library.cpp)

# import library test
add_executable(myexe main.cpp)
target_link_libraries(myexe "${CMAKE_SOURCE_DIR}/build/libdatabase_blocks.a")





