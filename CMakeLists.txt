cmake_minimum_required(VERSION 2.6)
set(CXX "g++-4.2")
project (libego)

if()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_COMPILER_IS_GNUCXX)
  #message("Linux and g++ detected")

  set (CMAKE_BUILD_TYPE "Release")

  set (MY_CXX_FLAGS "-ggdb3 -Wall -Wextra -Wswitch-enum")
  set (MY_CXX_FLAGS_OPT 
       "-O3 -march=native -fomit-frame-pointer -frename-registers -ffast-math")
  set (CMAKE_CXX_FLAGS_RELEASE "${MY_CXX_FLAGS} ${MY_CXX_FLAGS_OPT}")
  set (CMAKE_CXX_COMPILER "g++-4.2")
  #message (FATAL_ERROR "${CMAKE_CXX_COMPILER} ${CMAKE_CXX_FLAGS_RELEASE}")

else()
  message(FATAL_ERROR "${CMAKE_SYSTEM_NAME} not supported")
endif()

add_subdirectory (ego)
add_subdirectory (example)
