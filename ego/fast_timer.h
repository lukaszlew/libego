#ifndef _FAST_TIMER_H_
#define _FAST_TIMER_H_

#include "utils.h"

class FastTimer {
public:
  static volatile uint64 get_cc_time ();

  FastTimer ();
  void   reset ();
  void   start ();
  void   stop ();
  double ticks ();
  string to_string (float unit = 1.0);

private:
  double  sample_cnt;
  double  sample_sum;
  uint64  start_time;
  double  overhead;
};

#endif
