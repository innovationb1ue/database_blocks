#add_library(lib_uuid STATIC uuid.h other.h)
#set_target_properties(lib_uuid PROPERTIES LINKER_LANGUAGE CXX)

include_directories(${PROJECT_SOURCE_DIR}/libs)
add_subdirectory(spdlog-1.11.0)





