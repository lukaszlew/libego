/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of Library of Effective GO routines - EGO library      *
 *                                                                           *
 *  Copyright 2006 and onwards, Lukasz Lew                                   *
 *                                                                           *
 *  EGO library is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation; either version 2 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  EGO library is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with EGO library; if not, write to the Free Software               *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor,                           *
 *  Boston, MA  02110-1301  USA                                              *
 *                                                                           *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "fast_timer.h"

FastTimer::FastTimer () {
  reset ();
  uint64 t1, t2;
  t1 = get_cc_time ();
  t2 = get_cc_time ();
  overhead = double (t2 - t1);
}

// http://stackoverflow.com/questions/771867/how-to-make-a-cross-platform-c-inline-assembly-language/

uint64 FastTimer::get_cc_time () {
  if (sizeof(long) == 8) {
    uint64 a, d;
    asm volatile ("rdtsc\n\t" : "=a"(a), "=d"(d));
    return (d << 32) | (a & 0xffffffff);
  } else {
    uint64 l;
    asm volatile ("rdtsc\n\t" : "=A"(l));
    return l;
  }
}

void FastTimer::reset () {
  sample_cnt = 0;
  sample_sum = 0;
}

void FastTimer::start () {
  start_time = get_cc_time ();
}

void FastTimer::stop () {
  uint64 stop_time;
  stop_time = get_cc_time ();
  sample_cnt += 1.0;
  sample_sum += double (stop_time - start_time) - overhead;
}

double FastTimer::ticks () { return sample_sum / sample_cnt; }

string FastTimer::to_string (float unit) {
  ostringstream s;
  s.precision(15);
  s << "avg CC = " << ticks () / unit << " (cnt = " << sample_cnt << ")";
  return s.str ();
}
