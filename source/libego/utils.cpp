//
// Copyright 2006 and onwards, Lukasz Lew
//

// TODO move such stuff to porting.h (or somwhere)
#if defined(_MSC_VER) || defined(__MINGW32__)
#include <windows.h>
#else 
#include <sys/resource.h>
#endif

#include <iostream>
#include <cstdlib>

#include "utils.hpp"

// some usefull functions

// TODO use this to port rusage to windows/mingw
// http://octave.sourceforge.net/doxygen/html/getrusage_8cc-source.html
// http://stackoverflow.com/questions/771944/how-to-measure-user-time-used-by-process-on-windows
// http://www.ginac.de/pipermail/ginac-list/2006-July/000861.html

float process_user_time () {

// TODO ifdef POSIX ?
#ifndef WIN32

  rusage usage [1];
  getrusage (RUSAGE_SELF, usage);
  return 
    float(usage->ru_utime.tv_sec) +
    float(usage->ru_utime.tv_usec) / 1000000.0;

#elif defined(_MSC_VER) || defined(__MINGW32__)

  union { FILETIME t; __int64 i64; } start, exit, kernel, user;
  if (GetProcessTimes(GetCurrentProcess(), &start.t, &exit.t, &kernel.t, &user.t)) {
    __int64 userMicro = user.i64 / 10U; //(*((__int64*) &user.t)) / 10U;
    return (float)((double)userMicro / 1000000.0f);
  } else {
    return 0;
  }

#else

 return 0;

#endif

}
