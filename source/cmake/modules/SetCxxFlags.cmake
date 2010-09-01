macro (set_cxx_flags strong_release_flags)
  if(CMAKE_COMPILER_IS_GNUCXX)

    CHECK_CXX_COMPILER_FLAG ("-march=native" GNUCXX_SUPPORTS_MARCH_NATIVE)
    CHECK_CXX_COMPILER_FLAG ("-march=i686"   GNUCXX_SUPPORTS_MARCH_I686)

    # Set CMAKE_CXX_MARCH_FLAGS 
    if (GNUCXX_SUPPORTS_MARCH_NATIVE)
      set (CMAKE_CXX_MARCH_FLAGS "-march=native")
    elseif (GNUCXX_SUPPORTS_MARCH_I686)
      set (CMAKE_CXX_MARCH_FLAGS "-march=i686")
    else ()
      message ("Please upgrade your compiler")
      set (CMAKE_CXX_MARCH_FLAGS "")
    endif()

    set (CMAKE_CXX_FLAGS "-std=c++0x -ggdb3 -Wall -Wextra -Wswitch-enum -Wunused -fshow-column")

    set (STRONG "${strong_release_flags}")
    if (STRONG)
      set (CMAKE_CXX_FLAGS_RELEASE
        "-O3 -fomit-frame-pointer -frename-registers ${CMAKE_CXX_MARCH_FLAGS}")
    else()
      set (CMAKE_CXX_FLAGS_RELEASE "")
    endif()

    set (CMAKE_CXX_FLAGS_DEBUG "-fno-inline -DTESTING")

  else()

    message("${CMAKE_SYSTEM_NAME}  detected.")
    message ("If it works/not works for you, let us know libego-devel@googlegroups.com")

  endif()

endmacro (set_cxx_flags)
