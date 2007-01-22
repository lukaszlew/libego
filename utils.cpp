/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of GoBoard library,                                    *
 *                                                                           *
 *  Copyright 2006 Lukasz Lew                                                *
 *                                                                           *
 *  GoBoard library is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation; either version 2 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  GoBoard library is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with GoBoard library; if not, write to the Free Software           *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor,                           *
 *  Boston, MA  02110-1301  USA                                              *
 *                                                                           *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <iostream>
#include <fstream>
#include <sstream>

#include <cassert>
#include <cmath>

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

using namespace std;

typedef unsigned int uint;
typedef unsigned long long uint64;

#define qqe(...) fprintf(stderr, __VA_ARGS__)
#define qq(...) fprintf(stdout, __VA_ARGS__)

#define unused(p) (void)(p)
#define nop unused(0)

#define rep(i,n)     for (int i = 0;   i < (int)(n); i++)
#define seq(i,a,b)   for (int i = (a); i <= (b); i++)
#define dseq(i,b,a)  for (int i = (b); i >= (a); i--)

#define var(a,b) typeof(b) a=(b)
#define for_each(it, c) for(var(it,(c).begin()); it!=(c).end(); ++it)

// TODO can it be a function?
#define swap(a, b) \
  do {\
    typeof(a) tmp_a = a;\
    typeof(b) tmp_b = b;\
    b = tmp_a;\
    a = tmp_b;\
  } while (0);


// TODO can it be a function?
#define get_cc_cnt(tsc)  __asm__ __volatile__("rdtsc" : "=A" (tsc) : : "edx")

// TODO can it be a function?
#define cc_time_of(t ,instr) \
  do {\
    unsigned int cc_start, cc_stop;\
    get_cc_cnt(cc_start);\
    instr;\
    get_cc_cnt(cc_stop);\
    t += cc_stop - cc_start - 43;\
  } while (0)

// TODO can it be a function?
#define assertc(aspect, expr) assert((aspect) ? (expr) : true)

// namespace pm

const bool pm_ac = true;

namespace pm {             // Park - Miller "minimal standard"
  static unsigned long seed = 12345;

  void srand (unsigned long _seed) { seed = _seed; }

  const int cnt = (1<<31) - 1;  

  unsigned long rand_int () {       // a number between  0 ... cnt - 1
    unsigned long hi, lo;
    lo = 16807 * (seed & 0xffff);
    hi = 16807 * (seed >> 16);
    lo += (hi & 0x7fff) << 16;
    lo += hi >> 15;
    seed = (lo & 0x7FFFFFFF) + (lo >> 31);
    return seed;
  }

  // n must be between 1 .. (1<<16) + 1
  inline unsigned long rand_int (uint n) { // 0 .. n-1
    assertc (pm_ac, n > 0);
    assertc (pm_ac, n <= (1<<16)+1);
    return ((rand_int () & 0xffff) * n) >> 16;
  }

  void test () {
    uint start = pm::rand_int ();
    
    uint n = 1;
    uint max = 0;
    uint sum = start;
    
    while (true) {
      uint r = pm::rand_int ();
      if (r == start) break;
      n++;
      sum += r;
      if (max < r) max = r;
    }
    printf ("n = %d\n", n);
    printf ("max = %d\n", max);
    printf ("sum = %d\n", sum);
  }

  void test2 (uint k, uint n) {
    uint bucket [k];

    rep (ii, k)  bucket [ii] = 0;
    rep (ii, n) {
      uint r = rand_int (k);
      assert (r < k);
      bucket [r] ++;
    }
    rep (ii, k)  printf ("%d\n", bucket [ii]);
  }

}

const float infinity = 1000000000.0;

char getc_non_space (istream& is) {
  char c;
  is >> c;
  if (c == ' ' || c == '\t') return getc_non_space (is);
  return c;
}

float get_seconds () {
  rusage usage [1];
  getrusage (RUSAGE_SELF, usage);
  return float(usage->ru_utime.tv_sec) + float(usage->ru_utime.tv_usec) / 1000000.0;
}
