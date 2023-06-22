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
        test_normal.cc
)
target_link_libraries(
        hello_test
        GTest::gtest_main
        # this source dir is project root dir.
        "${CMAKE_SOURCE_DIR}/build/libdatabase_blocks.a"
)

include(GoogleTest)
gtest_discover_tests(hello_test)