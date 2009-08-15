enable_testing ()
add_custom_target (check ALL ${CMAKE_CTEST_COMMAND})

macro (add_boost_cxx_test name)
  add_executable (${name} ${name}.cpp)
  target_link_libraries (${name} boost_unit_test_framework-mt) # TODO find lib
  add_test (${name} ${name})
  add_dependencies (check ${name})
endmacro (add_boost_cxx_test)
