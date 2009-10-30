//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef FAST_TIMER_H_
#define FAST_TIMER_H_

#include "utils.h"

class FastTimer {
public:
  FastTimer ();
  void Reset();
  void Start();
  void Stop();
  double Ticks();
  string ToString (float unit = 1.0);

private:
  static uint64 GetCcTime ();

  double  sample_cnt;
  double  sample_sum;
  uint64  start_time;
  double  overhead;
};

#endif
