# GTest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# test executables
add_executable(
        hello_test
        mem_tree_test.cc
        db_test.cpp
)

# link for libs
target_link_libraries(
        hello_test
        GTest::gtest_main
        # this source dir is project root dir.
#        "${CMAKE_SOURCE_DIR}/build/libdatabase_blocks.a"
        database_blocks
)

target_include_directories(
        hello_test
        PRIVATE
        ${CMAKE_SOURCE_DIR}
)


include(GoogleTest)
gtest_discover_tests(hello_test)