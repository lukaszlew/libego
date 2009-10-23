include_directories (${libgtp_SOURCE_DIR}/libgtp)
add_executable (gtp_example main.cpp)
target_link_libraries (gtp_example gtp) 

install (TARGETS gtp_example ${INSTALL_TARGETS_DEFAULT_ARGS})
