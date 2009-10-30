//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "fast_random.h"
#include "testing.h"
#include "cstdio"

#ifdef __MINGW32__
#include <time.h>
#endif

const int FastRandom::kBound = (uint(1)<<31) - 1;

//tr1::minstd_rand0 mt; // this is eqivalent when #include <tr1/random>


FastRandom::FastRandom (uint seed_) {
  seed = seed_; 
}

void FastRandom::set_seed (uint seed_) { 
  seed = seed_; 
}

uint FastRandom::get_seed () { 
  return seed; 
}

uint FastRandom::rand_int () {       // a number between  0 ... kBound - 1
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
uint FastRandom::rand_int (uint n) { // 0 .. n-1
  assertc (fast_random_ac, n > 0);
  assertc (fast_random_ac, n <= (1<<16)+1);
  return ((rand_int () & 0xffff) * n) >> 16;
}

void FastRandom::test () {
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

void FastRandom::test2 (uint k, uint n) {
  uint* bucket = new uint[k];

  rep (ii, k)  bucket [ii] = 0;
  rep (ii, n) {
    uint r = rand_int (k);
    assert (r < k);
    bucket [r] ++;
  }
  rep (ii, k)  printf ("%d\n", bucket [ii]);
  delete[] bucket;
}
