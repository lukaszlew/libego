#ifndef _CONFIG_H_
#define _CONFIG_H_

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

// consistency checking / debugging control

#ifndef DEBUG
#define DEBUG
#endif

#ifdef NDEBUG
#error "NDEBUG not allowed."
#endif

#include <cassert>

#define assertc(aspect, expr) assert((aspect) ? (expr) : true)

#ifdef TESTING
const bool all_tests = true;
#else 
const bool all_tests = false;
#endif

const bool fast_random_ac     = all_tests;
const bool stack_ac           = all_tests;
const bool board_ac           = all_tests;
const bool player_ac          = all_tests;
const bool color_ac           = all_tests;
const bool coord_ac           = all_tests;
const bool vertex_ac          = all_tests;
const bool nbr_cnt_ac         = all_tests;
const bool chain_ac           = all_tests;
const bool board_empty_v_ac   = all_tests;
const bool board_hash_ac      = all_tests;
const bool board_color_at_ac  = all_tests;
const bool board_nbr_cnt_ac   = all_tests;
const bool chain_at_ac        = all_tests;
const bool chain_next_v_ac    = all_tests;
const bool chains_ac          = all_tests;
const bool playout_ac         = all_tests;
const bool aaf_ac             = all_tests;
const bool uct_ac             = all_tests;
const bool tree_ac            = all_tests;
const bool pool_ac            = all_tests;
const bool gtp_ac             = all_tests;

#endif
