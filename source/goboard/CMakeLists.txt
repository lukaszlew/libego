set_cxx_flags (TRUE)

# Ensure BOARDSIZE is correct.

# TODO make automatic configuration file.

# if(NOT DEFINED BOARDSIZE OR NOT BOARDSIZE MATCHES [0-9]+)
#   set (BOARDSIZE 9)
# endif()

# add_definitions(-DBOARDSIZE=${BOARDSIZE})
# message("BOARDSIZE = ${BOARDSIZE}")

# Targets

include_directories (${libego_SOURCE_DIR}/utils)
add_library(ego ego.cpp)
target_link_libraries (ego utils)

#TODO install includes as well
#install (TARGETS ego ${INSTALL_TARGETS_DEFAULT_ARGS})
