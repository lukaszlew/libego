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

#pragma once

#include <iostream>
#include <fstream>
#include <cassert>
#include <cmath>

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

namespace pm {             // Park - Miller "minimal standard"
  static unsigned long seed = 12345;

  void srand (unsigned long _seed) { seed = _seed; }

  unsigned long rand () {
    unsigned long hi, lo;
    lo = 16807 * (seed & 0xffff);
    hi = 16807 * (seed >> 16);
    lo += (hi & 0x7fff) << 16;
    lo += hi >> 15;
    if (lo > 0x7fffffff) lo -= 0x7fffffff; // TODO is it ok?
    seed = (long) lo;
    return lo;
  }

  void test () {
    uint start = pm::rand ();
    uint64 i = 2;
    while (pm::rand() != start) i++;
    cout << i << endl;
  }

}

const float infinity = 1000000000.0;

char getc_non_space (istream& is) {
  char c;
  is >> c;
  if (c == ' ' || c == '\t') return getc_non_space (is);
  return c;
}
