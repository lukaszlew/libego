#ifndef _FAST_RANDOM_
#define _FAST_RANDOM_

class FastRandom {             // Park - Miller "minimal standard"

  static const int cnt;
  uint seed;

public:

  FastRandom (uint seed_ = 12345);
  void set_seed (uint seed_);
  uint get_seed ();

  uint rand_int ();

  // n must be between 1 .. (1<<16) + 1
  inline uint rand_int (uint n);

  void test ();
  void test2 (uint k, uint n);
};

#endif
