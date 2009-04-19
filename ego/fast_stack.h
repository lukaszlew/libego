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

template <typename elt_t, uint _max_size> class FastStack {
public:
  elt_t tab [_max_size];
  uint size;

  FastStack () { clear (); }

  void clear () { size = 0; }

  void check () const {
    assertc (stack_ac, size <= _max_size);
  }

  bool is_empty () const { return size == 0; }

  elt_t& top () { assertc (stack_ac, size > 0); return tab [size-1]; }

  void push_back (elt_t& elt) { tab [size++] = elt; check (); }
  elt_t& new_top() { size += 1; check();  return tab [size-1]; }


  elt_t pop_random (FastRandom& fr) {
    assertc (stack_ac, size > 0);
    uint idx = fr.rand_int (size);
    elt_t elt = tab [idx];
    size--;
    tab [idx] = tab [size];
    return elt;
  }

  void pop () { size--; check (); }

};

// TODO add iterators

#endif
