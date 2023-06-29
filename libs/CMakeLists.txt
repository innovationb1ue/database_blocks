# Use FetchContent to download GSL
include_directories(gsl/)

add_library(lib_uuid uuid.h)
target_include_directories(lib_uuid PRIVATE GSL-4.0.0)
set_target_properties(lib_uuid PROPERTIES LINKER_LANGUAGE CXX)





