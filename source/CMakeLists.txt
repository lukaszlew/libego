cmake_minimum_required(VERSION 2.6)
project (libego)

# Import some cmake modules.

set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

include (CheckCXXCompilerFlag)

include (EnsureOutOfSourceBuild)
include (EnsureBuildTypeIsChosen)
include (AddBoostCxxTest)
include (SetDefaultInstallationDirs)

# Add subdirectories.

add_subdirectory (libgtp)
add_subdirectory (utils)
add_subdirectory (libego)
add_subdirectory (engine)
