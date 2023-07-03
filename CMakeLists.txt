cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 20)

project(database_blocks)

add_subdirectory(libs)

add_subdirectory(src)

include_directories(database_blocks libs)
#target_link_libraries(database_blocks lib_uuid)


# build tests ?
add_subdirectory(test)






