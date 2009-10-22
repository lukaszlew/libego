if(NOT CMAKE_BUILD_TYPE OR NOT CMAKE_BUILD_TYPE MATCHES "Debug|Release")
  message(
    FATAL_ERROR
    "\nChoose the type of build, options are: Debug, Release."
    )
endif(NOT CMAKE_BUILD_TYPE OR NOT CMAKE_BUILD_TYPE MATCHES "Debug|Release")
