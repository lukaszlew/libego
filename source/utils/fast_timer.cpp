//
// Copyright 2006 and onwards, Lukasz Lew
//

// TODO move such stuff to porting.h (or somwhere)
#if defined(_MSC_VER) || defined(__MINGW32__)
#include <windows.h>
#else 
#include <sys/resource.h>
#include <sys/time.h>
#endif

#ifdef _MSC_VER
#include <intrin.h>
#pragma intrinsic(__rdtsc)
#endif

#include <iostream>
#include <cstdlib>

#include "fast_timer.hpp"

// -----------------------------------------------------------------------------

FastTimer::FastTimer () {
  Reset ();
  uint64 t1, t2;
  t1 = GetCcTime ();
  t2 = GetCcTime ();
  overhead = double (t2 - t1);
}


uint64 FastTimer::GetCcTime () {
# ifdef _MSC_VER
    return __rdtsc();
# else
    if (sizeof(long) == 8) {
      uint64 a, d;
      asm volatile ("rdtsc\n\t" : "=a"(a), "=d"(d));
      return (d << 32) | (a & 0xffffffff);
    } else {
      uint64 l;
      asm volatile ("rdtsc\n\t" : "=A"(l));
      return l;
    }
# endif //_MSC_VER
}


void FastTimer::Reset () {
  sample_cnt = 0;
  sample_sum = 0;
}


void FastTimer::Start () {
  start_time = GetCcTime ();
}


void FastTimer::Stop () {
  uint64 stop_time;
  stop_time = GetCcTime ();
  sample_cnt += 1.0;
  sample_sum += double (stop_time - start_time) - overhead;
}


double FastTimer::Ticks () {
  return sample_sum / sample_cnt;
}


string FastTimer::ToString (float unit) {
  ostringstream s;
  s.precision(15);
  s << "avg CC = " << Ticks () / unit << " (cnt = " << sample_cnt << ")";
  return s.str ();
}


int TimeSeed () {
  FastTimer timer;
  return (int)timer.GetCcTime();
}


float ProcessUserTime () {
# ifndef WIN32

    rusage usage [1];
    getrusage (RUSAGE_SELF, usage);
    return 
      float(usage->ru_utime.tv_sec) +
      float(usage->ru_utime.tv_usec) / 1000000.0;

# elif defined(_MSC_VER) || defined(__MINGW32__)

    union { FILETIME t; __int64 i64; } start, exit, kernel, user;
    if (GetProcessTimes(GetCurrentProcess(), &start.t, &exit.t, &kernel.t, &user.t)) {
      __int64 userMicro = user.i64 / 10U; //(*((__int64*) &user.t)) / 10U;
      return (float)((double)userMicro / 1000000.0f);
    } else {
      return 0;
    }

# else

   return 0;

# endif
}

// TODO use this to port rusage to windows/mingw
// http://octave.sourceforge.net/doxygen/html/getrusage_8cc-source.html
// http://stackoverflow.com/questions/771944/how-to-measure-user-time-used-by-process-on-windows
// http://www.ginac.de/pipermail/ginac-list/2006-July/000861.html

