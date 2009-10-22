include (SetStrongCxxFlags)

find_package (Boost COMPONENTS date_time filesystem REQUIRED)
include_directories (${Boost_INCLUDE_DIRS})
link_directories    (${Boost_LIBRARY_DIRS})

include_directories (${libego_SOURCE_DIR}/libego)
link_directories    (${libego_BINARY_DIR}/libego)

include_directories (${libego_SOURCE_DIR}/libgtp)
link_directories    (${libego_BINARY_DIR}/libgtp)

add_executable (engine main.cpp logger.cpp)

target_link_libraries (engine ego gtp ${Boost_LIBRARIES}) 

install (TARGETS engine ${INSTALL_TARGETS_DEFAULT_ARGS})
