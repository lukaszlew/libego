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

// standard macros

#define qq(...) fprintf(stdout, __VA_ARGS__); fflush (stdout);

//TODO rename to ignore
#define unused(p) (void)(p)
#define nop unused(0)

#define rep(i,n)     for (int i = 0;   i < (int)(n); i++)
#define seq(i,a,b)   for (let (i, a); i <= (b); i++)
#define dseq(i,b,a)  for (let (i, b); i >= (a); i--)

#define let(a,b) typeof(b) a=(b)
#define for_each(it, c) for(let(it,(c).begin()); it!=(c).end(); ++it)

#define assertc(aspect, expr) assert((aspect) ? (expr) : true)

const float large_float = 1000000000000.0;

// PerformanceTimer

class PerformanceTimer {
  double  sample_cnt;
  double  sample_sum;
  uint64  start_time;
  double  overhead;
public:

  PerformanceTimer () {
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
#define cc_measure(cc_clock, instr) { cc_clock.start (); instr; cc_clock.stop (); }


// class PmRandom


class PmRandom {             // Park - Miller "minimal standard" 

  static const int cnt = (uint(1)<<31) - 1;  

  uint seed;
  //tr1::minstd_rand0 mt; // this is eqivalent when #include <tr1/random>

public:

  PmRandom (uint seed_ = 12345) //: mt (seed_) 
  { seed = seed_; }

  void set_seed (uint _seed) { seed = _seed; }
  uint get_seed () { return seed; }

  uint rand_int () {       // a number between  0 ... cnt - 1
    uint hi, lo;
    lo = 16807 * (seed & 0xffff);
    hi = 16807 * (seed >> 16);
    lo += (hi & 0x7fff) << 16;
    lo += hi >> 15;
    seed = (lo & 0x7FFFFFFF) + (lo >> 31);
    return seed;
    //return mt (); // equivalen
  }

  // n must be between 1 .. (1<<16) + 1
  inline uint rand_int (uint n) { // 0 .. n-1
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


// TODO use Stack in Board
template <typename elt_t, uint _max_size> class Stack {
public:
  elt_t tab [_max_size];
  uint size;

  Stack () {
    size = 0;
  }

  void check () const { 
    assertc (stack_ac, size <= _max_size);
  }

  bool is_empty () const { return size == 0; }

  elt_t& top () { assertc (stack_ac, size > 0); return tab [size-1]; }

  void   push_back (elt_t& elt) { tab [size++] = elt; check (); }


  elt_t pop_random (PmRandom& pm) { 
    assertc (stack_ac, size > 0); 
    uint idx = pm.rand_int (size);
    elt_t elt = tab [idx];
    size--;
    tab [idx] = tab [size];
    return elt; 
  }

  void   pop () { size--; check (); }

};


// very simple and useful FastMap

template <typename idx_t, typename elt_t> class FastMap {
  elt_t tab [idx_t::cnt];
public:
  elt_t& operator[] (idx_t pl)             { return tab [pl.get_idx ()]; }
  const elt_t& operator[] (idx_t pl) const { return tab [pl.get_idx ()]; }
};


// some usefull functions


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


// string/stream opereations

template <typename T> 
string to_string (T f) {
  ostringstream s;
  s << f;
  return s.str ();
}

char getc_non_space (istream& is) {
  char c;
  is.get(c);
  if (c == ' ' || c == '\t') return getc_non_space (is);
  return c;
}

bool is_all_whitespace (string s) {
  for_each (cp, s) 
    if (!isspace (*cp))
      return false;
  return true;
}

void remove_empty_lines (string* s) {
  istringstream in (*s);
  ostringstream out;
  string line;
  while (getline (in, line)) {
    if (is_all_whitespace (line)) continue;
    out << line << endl;
  }
  *s = out.str ();
}

void remove_trailing_whitespace (string* str) {
  while (isspace ( *(str->end ()-1) ))
    str->resize (str->size () - 1);
}

// g++ extensions


#if 0

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#else

#define likely(x)   (x)
#define unlikely(x) (x)

#endif



#if 1

#define no_inline   __attribute__((noinline))
#define flatten     __attribute__((flatten))
#define all_inline  __attribute__((always_inline))

#else

#define no_inline
#define flatten  
#define all_inline

#endif
