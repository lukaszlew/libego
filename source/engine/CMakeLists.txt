include_directories (${libego_SOURCE_DIR}/libego)
link_directories    (${libego_BINARY_DIR}/libego)

include_directories (${libego_SOURCE_DIR}/libgtp)
link_directories    (${libego_BINARY_DIR}/libgtp)

add_executable (engine main.cpp)

target_link_libraries (engine ego gtp) 

install (TARGETS engine ${INSTALL_TARGETS_DEFAULT_ARGS})
