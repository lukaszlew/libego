cmake_minimum_required(VERSION 2.6)
project (libego)

if()
elseif((CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "Darwin") AND CMAKE_COMPILER_IS_GNUCXX)
  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message("Linux and g++ detected.")
  elseif(CMAKE_SYSTEM_MAKE STREQUAL "Darwin")
    message("Darwin and g++ detected.")
  endif()

  if(${BOARDSIZE} MATCHES [0-9]+)
    message ("Custom board size: ${BOARDSIZE}")
    add_definitions(-DBOARDSIZE=${BOARDSIZE})
  else()
    message ("Default board size: 9")
    add_definitions(-DBOARDSIZE=9)
  endif()

  set (MY_CXX_FLAGS 
    "-ggdb3 -Wall -Wextra -Wswitch-enum -Wunused")
  set (ALL_CXX_FLAGS_RELEASE "-O3 -fomit-frame-pointer -frename-registers -ffast-math")
  if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set (MY_CXX_FLAGS_RELEASE "-march=native ${ALL_CXX_FLAGS_RELEASE}")
  else()
    set (MY_CXX_FLAGS_RELEASE ${ALL_CXX_FLAGS_RELEASE})
  endif()
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
