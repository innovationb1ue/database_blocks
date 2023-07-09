SET(database_blocks_source_files mem_tree_impl.h mem_tree_impl.cpp db.h config.h db.cpp wal.h wal.cpp wal_manager.cpp wal_manager.h)

include_directories(../libs)

include_directories(../libs/spdlog-1.11.0/include)

if (NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif ()

add_library(database_blocks ${database_blocks_source_files})

