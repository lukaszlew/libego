//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifdef _MSC_VER
#include <intrin.h>
#pragma intrinsic(__rdtsc)
#endif


#include "fast_timer.h"

FastTimer::FastTimer () {
  Reset ();
  uint64 t1, t2;
  t1 = GetCcTime ();
  t2 = GetCcTime ();
  overhead = double (t2 - t1);
}

// TODO: http://stackoverflow.com/questions/771867/how-to-make-a-cross-platform-c-inline-assembly-language/

uint64 FastTimer::GetCcTime () {
#ifdef _MSC_VER
  return __rdtsc();
#else
  if (sizeof(long) == 8) {
    uint64 a, d;
    asm volatile ("rdtsc\n\t" : "=a"(a), "=d"(d));
    return (d << 32) | (a & 0xffffffff);
  } else {
    uint64 l;
    asm volatile ("rdtsc\n\t" : "=A"(l));
    return l;
  }
#endif //_MSC_VER
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

double FastTimer::Ticks () { return sample_sum / sample_cnt; }

string FastTimer::ToString (float unit) {
  ostringstream s;
  s.precision(15);
  s << "avg CC = " << Ticks () / unit << " (cnt = " << sample_cnt << ")";
  return s.str ();
}
