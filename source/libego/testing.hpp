//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef TESTING_H_
#define TESTING_H_

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
const bool pool_ac            = all_tests;

#endif
