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

class RdtscTimer {
  double  sample_cnt;
  double  sample_sum;
  uint64  start_time;
  double  overhead;
public:

  RdtscTimer () {
    reset ();
    uint64 t1, t2;
    t1 = get_cc_time ();
    t2 = get_cc_time ();
    overhead = double (t2 - t1);
  }

  void reset () {
    sample_cnt = 0;
    sample_sum = 0;
  }

  uint64 get_cc_time () volatile {
    uint64 ret;
    __asm__ __volatile__("rdtsc" : "=A" (ret) : :);
    return ret;
  }

  void start () {
    start_time = get_cc_time ();
  }

  void stop () {
    uint64 stop_time;
    stop_time = get_cc_time ();
    sample_cnt += 1.0;
    sample_sum += double (stop_time - start_time) - overhead;
  }

  double ticks () { return sample_sum / sample_cnt; }

  string to_string (float unit = 1.0) {
    ostringstream s;
    s.precision(15);
    s << "avg CC = " << ticks () / unit << " (cnt = " << sample_cnt << ")";
    return s.str ();
  }
};
