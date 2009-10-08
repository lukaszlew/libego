enable_testing ()

find_package (Boost COMPONENTS unit_test_framework REQUIRED)

add_custom_target (check ALL ${CMAKE_CTEST_COMMAND})

macro (add_boost_cxx_test name)
  link_directories ( ${Boost_LIBRARY_DIRS} )
  include_directories ( ${Boost_INCLUDE_DIRS} )

  add_executable (${name} ${name}.cpp)
  target_link_libraries (${name} ${Boost_LIBRARIES})

  add_test (${name} ${name})
  add_dependencies (check ${name})
endmacro (add_boost_cxx_test)
