#ifndef FAST_RANDOM_
#define FAST_RANDOM_

#include "utils.h"

class FastRandom {             // Park - Miller "minimal standard"
public:

  FastRandom (uint seed_);
  void set_seed (uint seed_);
  uint get_seed ();

  uint rand_int ();

  // n must be between 1 .. (1<<16) + 1
  uint rand_int (uint n);
  
  void test ();
  void test2 (uint k, uint n);

  static const int kBound;
  uint seed;
};

#endif
