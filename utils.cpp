/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of Library of Effective GO routines - EGO library      *
 *                                                                           *
 *  Copyright 2006, 2007 Lukasz Lew                                          *
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

typedef unsigned int uint;
typedef unsigned long long uint64;

//#define qqe(...) fprintf(stderr, __VA_ARGS__)
#define qq(...) fprintf(stdout, __VA_ARGS__); fflush (stdout);

#define unused(p) (void)(p)
#define nop unused(0)

#define rep(i,n)     for (int i = 0;   i < (int)(n); i++)
#define seq(i,a,b)   for (int i = (a); i <= (b); i++)
#define dseq(i,b,a)  for (int i = (b); i >= (a); i--)

#define let(a,b) typeof(b) a=(b)
#define for_each(it, c) for(let(it,(c).begin()); it!=(c).end(); ++it)

// TODO can it be a function?
#define swap(a, b) \
  do {\
    typeof(a) tmp_a = a;\
    typeof(b) tmp_b = b;\
    b = tmp_a;\
    a = tmp_b;\
  } while (0);


class cc_clock_t {
  double  sample_cnt;
  double  sample_sum;
  uint    start_time;
  double  overhead;
public:

  cc_clock_t () : sample_cnt (0), sample_sum (0) { 
    uint t1, t2;
    get_cc_time (t1);
    get_cc_time (t2);
    overhead = double (t2 - t1);
  }

  void get_cc_time (uint& t) {
    __asm__ __volatile__("rdtsc" : "=A" (t) : : "edx");
  }

  void start () {
    get_cc_time (start_time);
  }

  void stop () {
    uint stop_time;
    get_cc_time (stop_time);
    sample_cnt += 1.0;
    sample_sum += double (stop_time - start_time) - overhead;
  }

  string to_string () {
    ostringstream s;
    s << "avg CC = " << sample_sum / sample_cnt << " (cnt = " << sample_cnt << ")";
    return s.str ();
  }
};

#define cc_measure(cc_clock, instr) { cc_clock.start (); instr; cc_clock.stop (); }

// TODO can it be a function?
#define assertc(aspect, expr) assert((aspect) ? (expr) : true)


// namespace pm

namespace random_pm_aux {
  const int cnt = (1<<31) - 1;  
}

class random_pm_t {             // Park - Miller "minimal standard"

  unsigned long seed;

public:

  random_pm_t (unsigned long seed_ = 12345) { seed = seed_; }

  void srand (unsigned long _seed) { seed = _seed; }

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
    uint start = rand_int ();
    
    uint n = 1;
    uint max = 0;
    uint sum = start;
    
    while (true) {
      uint r = rand_int ();
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

};

const float large_float = 1000000000000.0;

template <typename elt_t, uint _max_size> class stack_t {
public:
  elt_t tab [_max_size];
  uint size;

  stack_t () {
    size = 0;
  }

  void check () const { 
    assertc (stack_ac, size <= _max_size);
  }

  bool is_empty () const { return size == 0; }

  elt_t& top () { assertc (stack_ac, size > 0); return tab [size-1]; }

  void   push_back (elt_t& elt) { tab [size++] = elt; check (); }


  elt_t pop_random (random_pm_t& pm) { 
    assertc (stack_ac, size > 0); 
    uint idx = pm.rand_int (size);
    elt_t elt = tab [idx];
    size--;
    tab [idx] = tab [size];
    return elt; 
  }

  void   pop () { size--; check (); }

};

char getc_non_space (istream& is) {
  char c;
  is.get(c);
  if (c == ' ' || c == '\t') return getc_non_space (is);
  return c;
}

float get_seconds () {
  rusage usage [1];
  getrusage (RUSAGE_SELF, usage);
  return float(usage->ru_utime.tv_sec) + float(usage->ru_utime.tv_usec) / 1000000.0;
}

void fatal_error (const char* s) {
  cerr << "Fatal error: " << s << endl;
  assert (false);
  exit (1);
}

#if 1

#define no_inline   __attribute__((noinline))
#define flatten     __attribute__((flatten))
#define all_inline  __attribute__((always_inline))

#else

#define no_inline
#define flatten  
#define all_inline

#endif
