set_cxx_flags (TRUE)

include_directories (${libego_SOURCE_DIR}/utils)
link_directories    (${libego_BINARY_DIR}/utils)

include_directories (${libego_SOURCE_DIR}/goboard)
link_directories    (${libego_BINARY_DIR}/goboard)

include_directories (${libego_SOURCE_DIR}/gtp)
link_directories    (${libego_BINARY_DIR}/gtp)

add_executable (engine main.cpp time_control.cpp mcts_playout.cpp mcts_tree.cpp)

target_link_libraries (engine utils ego gtp)

install (TARGETS engine ${INSTALL_TARGETS_DEFAULT_ARGS})
