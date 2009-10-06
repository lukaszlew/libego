#ifndef _FAST_RANDOM_
#define _FAST_RANDOM_

#include "utils.h"

class FastRandom {             // Park - Miller "minimal standard"

  static const int cnt;
  uint seed;

public:

  FastRandom (uint seed_);
  void set_seed (uint seed_);
  uint get_seed ();

  uint rand_int ();

  // n must be between 1 .. (1<<16) + 1
  uint rand_int (uint n);
  
  void test ();
  void test2 (uint k, uint n);
};

extern FastRandom global_random;

#endif
