include_directories (${libego_SOURCE_DIR}/ego)

link_directories (${libego_BINARY_DIR}/ego)

add_executable (engine main.cpp)

target_link_libraries (engine ego) 
