#ifndef _FAST_STACK_H_
#define _FAST_STACK_H_

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

#include "utils.h"
#include "testing.h"
#include "fast_random.h"

template <typename Elt, uint max_size> class FastStack {
public:
  FastStack () : size (0) {
  }

  void Clear () {
    size = 0;
  }

  bool IsEmpty () const {
    return size == 0;
  }

  uint Size() {
    return size;
  }
  
  Elt* Data() {
    return tab;
  }

  void Push (Elt& elt) {
    tab [size++] = elt;
    Check ();
  }

  Elt& Top () {
    assertc (stack_ac, size > 0);
    return tab [size-1];
  }

  Elt& NewTop() {
    size += 1;
    Check();
    return Top();
  }

  void Pop () {
    size--;
    Check ();
  }

  Elt& PopTop () {
    size--;
    Check();
    return tab [size];
  }

  Elt PopRandom (FastRandom& fr) {
    assertc (stack_ac, size > 0);
    uint idx = fr.rand_int (size);
    Elt elt = tab [idx];
    size--;
    tab [idx] = tab [size];
    return elt;
  }

  Elt& operator[] (uint i) { 
    assertc (stack_ac, i < size);
    return tab [i];
  }

  const Elt& operator[] (uint i) const {
    assertc (stack_ac, i < size);
    return tab [i];
  }

private:
  void Check () const {
    assertc (stack_ac, size <= max_size);
  }

private:
  Elt tab [max_size];
  uint size;
};

// TODO add iterators

#endif
