//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef CONFIG_H_
#define CONFIG_H_

// TODO this have to be renamed to max_board_size
// TODO better use of CMake

#ifdef BOARDSIZE
const uint board_size = BOARDSIZE;
#undef BOARDSIZE
#else
const uint board_size = 9;
#endif

#endif
