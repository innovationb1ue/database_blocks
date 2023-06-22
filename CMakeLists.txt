cmake_minimum_required(VERSION 3.24)
project(database_blocks)

set(CMAKE_CXX_STANDARD 17)

add_library(database_blocks mem_tree_impl.cpp)

# import library test
add_executable(myexe main.cpp)
target_link_libraries(myexe "${CMAKE_SOURCE_DIR}/build/libdatabase_blocks.a")

# GTest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(
        hello_test
        test/test_normal.cc
)
target_link_libraries(
        hello_test
        GTest::gtest_main
        "${CMAKE_SOURCE_DIR}/build/libdatabase_blocks.a"
)

include(GoogleTest)
gtest_discover_tests(hello_test)




