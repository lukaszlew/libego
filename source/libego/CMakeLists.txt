find_package (Boost COMPONENTS date_time REQUIRED)

include_directories (${libego_SOURCE_DIR}/libgtp)
link_directories    (${libego_BINARY_DIR}/libgtp)

add_library(ego ego.cpp)
target_link_libraries (ego)

#TODO install includes as well
#install (TARGETS ego ${INSTALL_TARGETS_DEFAULT_ARGS})
