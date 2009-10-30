//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef FAST_POOL_H_
#define FAST_POOL_H_

#include "fast_stack.h"
#include "utils.h"
#include "testing.h"

template <class Elt, uint size>
class FastPool {
public:

  FastPool () {
    memory = new Elt[size];
    free_elts = new FreeEltsStack;
    Reset ();
  }

  ~FastPool () {
    delete [] memory;
    delete free_elts;
  }

  void Reset() {
    free_elts->Clear(); // TODO tu skonczylem
    rep (i, size) {
      free_elts->Push (memory + i);
    }
  }

  Elt* Alloc () { 
    return new (free_elts->PopTop ()) Elt;
  }

  void Free (Elt* elt) { 
    elt-~Elt();
    free_elts->Push (elt);
  }

private:

  typedef FastStack<Elt*, size> FreeEltsStack;

private:
  Elt* memory;
  FreeEltsStack* free_elts;
};

#endif
