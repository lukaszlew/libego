include_directories (${libego_SOURCE_DIR}/libgtp)
link_directories    (${libego_BINARY_DIR}/libgtp)

add_library(ego ego.cpp)

#TODO install includes as well
#install (TARGETS ego ${INSTALL_TARGETS_DEFAULT_ARGS})
