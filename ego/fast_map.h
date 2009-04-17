#ifndef _FAST_MAP_H_
#define _FAST_MAP_H_

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

// very simple and useful FastMap

template <typename idx_t, typename elt_t>
class FastMap {

  elt_t tab [idx_t::cnt];

public:

  elt_t& operator[] (idx_t pl) { 
    return tab [pl.get_idx ()];
  }

  const elt_t& operator[] (idx_t pl) const {
    return tab [pl.get_idx ()];
  }

  void memset(uint val) { 
    memset(tab, val, sizeof(elt_t)*idx_t::cnt); 
  }

  void SetAll(const elt_t& val) {
    rep (ii, idx_t::cnt)
      tab[ii] = val;
  }
};

// TODO add iterators

#endif
