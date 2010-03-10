set_cxx_flags (FALSE)

include_directories (${libego_SOURCE_DIR}/utils)
include_directories (${libego_SOURCE_DIR}/goboard)
include_directories (${libego_SOURCE_DIR}/gtp)
include_directories (${libego_SOURCE_DIR}/engine)

add_executable (engine main.cpp) #TODO make mm_train separate executable
target_link_libraries (engine ai)

install (TARGETS engine ${INSTALL_TARGETS_DEFAULT_ARGS})
