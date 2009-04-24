cmake_minimum_required(VERSION 2.6)
set(CXX "g++-4.2")
project (libego)

if()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_COMPILER_IS_GNUCXX)

  message("Linux and g++ detected")

  # comment this line if you don't have g++-4.2
  set (CMAKE_CXX_COMPILER "g++-4.2")

  set (MY_CXX_FLAGS 
    "-ggdb3 -Wall -Wextra -Wswitch-enum -Wunused")

  set (MY_CXX_FLAGS_RELEASE
    "-O3 -march=native -fomit-frame-pointer -frename-registers -ffast-math")

  set (MY_CXX_FLAGS_DEBUG "-fno-inline -DTESTING")

  set (CMAKE_CXX_FLAGS_DEBUG   "${MY_CXX_FLAGS} ${MY_CXX_FLAGS_DEBUG}")
  set (CMAKE_CXX_FLAGS_RELEASE "${MY_CXX_FLAGS} ${MY_CXX_FLAGS_RELEASE}")

else()

  message(FATAL_ERROR "${CMAKE_SYSTEM_NAME} not supported")

endif()

add_subdirectory (ego)
add_subdirectory (example)
