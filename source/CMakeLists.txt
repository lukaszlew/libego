cmake_minimum_required(VERSION 2.6)
project (libgtp) # Replace ... with your project

set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

include (EnsureOutOfSourceBuild)
include (AddBoostCxxTest)
include (SetDefaultInstallationDirs)

add_subdirectory (libgtp)
add_subdirectory (example)
