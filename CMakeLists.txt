cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 20)

project(database_blocks)

add_subdirectory(libs)

SET(database_blocks_source_files src/mem_tree_impl.cpp src/mem_tree_impl.h src/db.h src/config.h src/db.cpp src/wal.h src/wal.cpp util/uuid_util.h src/wal_manager.cpp src/wal_manager.h)

add_library(database_blocks ${database_blocks_source_files})

include_directories(database_blocks libs)
#target_link_libraries(database_blocks lib_uuid)


# build tests ?
add_subdirectory(test)






