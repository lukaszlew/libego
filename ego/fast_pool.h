#ifndef _FAST_POOL_H_
#define _FAST_POOL_H_

#include "utils.h"
#include "testing.h"

template <class elt_t> class FastPool {
public:
  FastPool (uint pool_size) {
    this->pool_size = pool_size;
    memory   = new elt_t  [pool_size];
    free_elt = new elt_t* [pool_size];
  }

  void reset() {
    free_elt_count = pool_size;
    rep (i, pool_size) free_elt [i] = memory + i;
  }

  ~FastPool () {
    delete [] memory;
    delete [] free_elt;
  }

  elt_t* malloc () { 
    assertc (pool_ac, free_elt_count > 0);
    elt_t* ret = free_elt [--free_elt_count];
    ret->PoolConstruct();
    return ret;
  }

  void free (elt_t* elt) { 
    free_elt [free_elt_count++] = elt;  
  }

private:
  uint pool_size;

  elt_t* memory;

  elt_t** free_elt;
  uint    free_elt_count;
};

#endif
