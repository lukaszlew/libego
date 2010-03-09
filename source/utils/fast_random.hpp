//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef FAST_RANDOM_
#define FAST_RANDOM_

#include "utils.hpp"

class FastRandom {             // Park - Miller "minimal standard"
public:

  FastRandom (uint seed);
  void SetSeed (uint seed);
  uint GetSeed ();

  uint GetNextUint ();
  uint GetNextUint (uint n);   // n must be between 1 .. (1<<16) + 1

  double NextDouble ();
  double NextDouble (double scale);

  uint seed;
};

#endif
