set_cxx_flags (TRUE)

include_directories (${libego_SOURCE_DIR}/utils)
include_directories (${libego_SOURCE_DIR}/goboard)
include_directories (${libego_SOURCE_DIR}/gtp)

add_library (ai time_control.cpp mcts_playout.cpp mcts_tree.cpp param.cpp)

target_link_libraries (ai ego gtp)

# install (TARGETS engine ${INSTALL_TARGETS_DEFAULT_ARGS})
