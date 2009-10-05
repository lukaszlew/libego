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

#include <fstream>

#include "basic_gtp.h"

#include "testing.h"

BasicGtp::BasicGtp (Gtp::Repl& gtp, FullBoard& board_) : board (board_) { 
  gtp.Register ("boardsize",    this, &BasicGtp::CBoardsize);
  gtp.Register ("clear_board",  this, &BasicGtp::CClear_board);
  gtp.Register ("komi",         this, &BasicGtp::CKomi);
  gtp.Register ("play",         this, &BasicGtp::CPlay);
  gtp.Register ("undo",         this, &BasicGtp::CUndo);
  gtp.Register ("showboard",    this, &BasicGtp::CShowboard);
}


void BasicGtp::CBoardsize (Gtp::Io& io) {
  int new_board_size = io.Read<int> ();
  if (new_board_size != int (board_size)) { 
    io.SetError ("unacceptable size");
    return;
  }
  io.CheckEmpty();
}

void BasicGtp::CClear_board (Gtp::Io& io) {
  io.CheckEmpty();
  board.clear ();
}

void BasicGtp::CKomi (Gtp::Io& io) {
  float new_komi = io.Read<float> ();
  io.CheckEmpty();
  board.set_komi (-new_komi);
}

void BasicGtp::CPlay (Gtp::Io& io) {
  Player pl = io.Read<Player> ();
  Vertex v  = io.Read<Vertex> ();
  io.CheckEmpty ();

  if (v != Vertex::resign () && board.try_play (pl, v) == false) {
    io.SetError ("illegal move");
    return;
  }
}

void BasicGtp::CUndo (Gtp::Io& io) {
  io.CheckEmpty ();
  if (board.undo () == false) {
    io.SetError ("too many undo");
    return;
  }
}
    
void BasicGtp::CShowboard (Gtp::Io& io) {
  io.CheckEmpty ();
  io.out << endl << board.board().to_string();
}
