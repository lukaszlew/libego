cmake_minimum_required(VERSION 2.6)
project (libego)

set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

include (EnsureOutOfSourceBuild)
include (AddBoostCxxTest)
include (CheckCXXCompilerFlag)

# Define BOARDSIZE macro.

if(NOT DEFINED BOARDSIZE OR NOT BOARDSIZE MATCHES [0-9]+)
  set (BOARDSIZE 9)
endif()

add_definitions(-DBOARDSIZE=${BOARDSIZE})
message("BOARDSIZE = ${BOARDSIZE}")

# Set compiler flags.

if()
elseif(CMAKE_COMPILER_IS_GNUCXX)

  message("${CMAKE_SYSTEM_NAME} and g++ detected.")

  CHECK_CXX_COMPILER_FLAG ("-march=native" GNUCXX_SUPPORTS_MARCH_NATIVE)
  CHECK_CXX_COMPILER_FLAG ("-march=i686"   GNUCXX_SUPPORTS_MARCH_I686)

  if ()
  elseif (GNUCXX_SUPPORTS_MARCH_NATIVE)
    set (CMAKE_CXX_MARCH_FLAGS "-march=native")
  elseif (GNUCXX_SUPPORTS_MARCH_I686)
    set (CMAKE_CXX_MARCH_FLAGS "-march=i686")
  else ()
    message ("Please upgrade your compiler")
    set (CMAKE_CXX_MARCH_FLAGS "")
  endif()

  set (CMAKE_CXX_FLAGS "-ggdb3 -Wall -Wextra -Wswitch-enum -Wunused")
  set (CMAKE_CXX_FLAGS_RELEASE "-O3 -fomit-frame-pointer -frename-registers -ffast-math ${CMAKE_CXX_MARCH_FLAGS}")
  set (CMAKE_CXX_FLAGS_DEBUG "-fno-inline -DTESTING")

else()

  message("${CMAKE_SYSTEM_NAME}  detected.")
  message ("If it works/not works for you, let us know libego-devel@googlegroups.com")

endif()

# Add subdirectories.

add_subdirectory (ego)
add_subdirectory (example)
add_subdirectory (libgtp)
