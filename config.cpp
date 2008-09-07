/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of Library of Effective GO routines - EGO library      *
 *                                                                           *
 *  Copyright 2006, 2007 Lukasz Lew                                          *
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


typedef unsigned int uint;
typedef unsigned long long uint64;

// constants

// board parameters
const float default_komi       = 7.5;
const uint board_size          = 9;
const uint board_area          = board_size * board_size;
const uint max_empty_v_cnt     = board_area;
const uint max_game_length     = board_area * 4;
const bool playout_print       = false;
const uint max_playout_length  = board_area * 2;

// mercy rule

const bool use_mercy_rule      = false;
const uint mercy_threshold     = 25;

// uct parameters

const float initial_value                 = 0.0;
const float initial_bias                  = 1.0;
const float mature_bias_threshold         = initial_bias + 100.0;
const float explore_rate                  = 1.0;
const uint  uct_max_depth                 = 1000;
const uint  uct_max_nodes                 = 1000000;
const float resign_value                  = 0.95;
const uint  uct_genmove_playout_cnt       = 50000;
const float print_visit_threshold_base    = 500.0;
const float print_visit_threshold_parent  = 0.02;


// consistency checking / debugging control


#ifdef NDEBUG
#error "NDEBUG not allowed."
#endif

#ifndef DEBUG
#error "DEBUG should be defined."
#endif

const bool pm_ac              = false;
const bool stack_ac           = false;

const bool paranoic           = false;
const bool board_ac           = false;

const bool player_ac          = paranoic;
const bool color_ac           = paranoic;
const bool coord_ac           = paranoic;
const bool vertex_ac          = paranoic;



const bool nbr_cnt_ac         = paranoic;
const bool chain_ac           = paranoic;
const bool board_empty_v_ac   = paranoic;
const bool board_hash_ac      = paranoic;
const bool board_color_at_ac  = paranoic;
const bool board_nbr_cnt_ac   = paranoic;
const bool chain_at_ac        = paranoic;
const bool chain_next_v_ac    = paranoic;
const bool chains_ac          = paranoic;

const bool playout_ac         = false;
const bool aaf_ac             = false;
const bool uct_ac             = false;
const bool tree_ac            = false;
const bool pool_ac            = false;
const bool gtp_ac             = true;
