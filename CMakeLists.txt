cmake_minimum_required(VERSION 2.6)
project ( ... ) # Replace ... with your project

set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

include (EnsureOutOfSourceBuild)
include (AddBoostCxxTest)

add_subdirectory ( ... ) # Replace ... with your subdirectory.
