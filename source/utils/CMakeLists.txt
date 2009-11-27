set_cxx_flags (FALSE)

find_package (Boost COMPONENTS date_time filesystem REQUIRED)
include_directories (${Boost_INCLUDE_DIRS})
link_directories    (${Boost_LIBRARY_DIRS})

add_library(utils logger.cpp test.cpp)
target_link_libraries (utils ${Boost_LIBRARIES})
