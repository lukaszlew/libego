#ifndef _FAST_POOL_H_
#define _FAST_POOL_H_

#include "utils.h"
#include "testing.h"

template <class elt_t> class FastPool {
public:
  FastPool (uint pool_size) {
    free_elt_count = pool_size;
    memory   = new elt_t  [pool_size];
    free_elt = new elt_t* [pool_size];
    rep (i, pool_size) free_elt [i] = memory + i;
  }

  ~FastPool () {
    delete [] memory;
    delete [] free_elt;
  }

  elt_t* malloc () { 
    assertc (pool_ac, free_elt_count > 0);
    return free_elt [--free_elt_count]; 
  }

  void free (elt_t* elt) { 
    free_elt [free_elt_count++] = elt;  
  }

private:
  elt_t* memory;

  elt_t** free_elt;
  uint    free_elt_count;
};

#endif
