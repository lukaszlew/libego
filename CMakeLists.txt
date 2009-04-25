cmake_minimum_required(VERSION 2.6)
project (libego)

if()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_COMPILER_IS_GNUCXX)

  message("Linux and g++ detected.")

  set (MY_CXX_FLAGS 
    "-ggdb3 -Wall -Wextra -Wswitch-enum -Wunused")
  set (MY_CXX_FLAGS_RELEASE
    "-O3 -march=native -fomit-frame-pointer -frename-registers -ffast-math")
  set (MY_CXX_FLAGS_DEBUG
    "-fno-inline -DTESTING")
  set (CMAKE_CXX_FLAGS_DEBUG
    "${MY_CXX_FLAGS} ${MY_CXX_FLAGS_DEBUG}")
  set (CMAKE_CXX_FLAGS_RELEASE
    "${MY_CXX_FLAGS} ${MY_CXX_FLAGS_RELEASE}")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")

  message ("Windows detected")
  message ("If it works for you, let us know libego-devel@googlegroups.com")

else()

  message(FATAL_ERROR "${CMAKE_SYSTEM_NAME} not supported")

endif()

add_subdirectory (ego)
add_subdirectory (example)
