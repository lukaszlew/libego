include (SetStrongCxxFlags)

# Ensure BOARDSIZE is correct.

if(NOT DEFINED BOARDSIZE OR NOT BOARDSIZE MATCHES [0-9]+)
  set (BOARDSIZE 9)
endif()

add_definitions(-DBOARDSIZE=${BOARDSIZE})
message("BOARDSIZE = ${BOARDSIZE}")

# Targets

find_package (Boost COMPONENTS date_time REQUIRED)

include_directories (${libego_SOURCE_DIR}/libgtp)
link_directories    (${libego_BINARY_DIR}/libgtp)

add_library(ego ego.cpp)
target_link_libraries (ego)

#TODO install includes as well
#install (TARGETS ego ${INSTALL_TARGETS_DEFAULT_ARGS})
