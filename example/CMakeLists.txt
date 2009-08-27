include_directories (${libego_SOURCE_DIR}/ego)
link_directories    (${libego_BINARY_DIR}/ego)

include_directories (${libego_SOURCE_DIR}/libgtp)
link_directories    (${libego_BINARY_DIR}/libgtp)

add_executable (engine main.cpp)

target_link_libraries (engine ego gtp) 
