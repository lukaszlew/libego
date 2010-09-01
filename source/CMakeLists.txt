cmake_minimum_required(VERSION 2.6)
project (libego)

# Import some cmake modules.

set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

include (CheckCXXCompilerFlag)

include (EnsureOutOfSourceBuild)
include (EnsureBuildTypeIsChosen)
# include (AddBoostCxxTest)
include (SetDefaultInstallationDirs)
include (SetCxxFlags)

# Add subdirectories.

add_subdirectory (utils)
add_subdirectory (gtp)
add_subdirectory (goboard)
add_subdirectory (engine)
add_subdirectory (main)
# add_subdirectory (libgamegui)
# add_subdirectory (gui)
