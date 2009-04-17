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

#include "color.h"
#include "testing.h"

Color::Color () {
  idx = -1;
} // TODO test - remove it

Color::Color (uint idx_) {
  idx = idx_; 
} // TODO test - remove it

Color::Color (Player pl) {
  idx = pl.get_idx ();
}

Color::Color (char c) {  // may return color_wrong_char
  switch (c) {
  case '#': idx = black_idx; break;
  case 'O': idx = white_idx; break;
  case '.': idx = empty_idx; break;
  case '*': idx = off_board_idx; break;
  default : idx = wrong_char_idx; break;
  }
}

void Color::check () const { 
  assertc (color_ac, (idx & (~3)) == 0); 
}

bool Color::is_player () const {
  return idx <= white_idx;      // & (~1)) == 0; }
} 

bool Color::is_not_player () const {
  return idx  > white_idx;      // & (~1)) == 0; }
}

Player Color::to_player () const {
  return Player (idx);
}

char Color::to_char () const { 
  switch (idx) {
  case black_idx:      return '#';
  case white_idx:      return 'O';
  case empty_idx:      return '.';
  case off_board_idx:  return ' ';
  default : assertc (color_ac, false);
  }
  return '?';                 // should not happen
}

bool Color::in_range () const {
  return idx < Color::cnt;
}

// TODO iterators?
void Color::next () {
  idx++;
}

uint Color::get_idx () const {
  return idx;
}

bool Color::operator== (Color other) const {
  return idx == other.idx;
}

bool Color::operator!= (Color other) const {
  return idx != other.idx;
}

Color Color::black ()      { return Color (black_idx); }
Color Color::white ()      { return Color (white_idx); }
Color Color::empty ()      { return Color (empty_idx); }
Color Color::off_board  () { return Color (off_board_idx); }
Color Color::wrong_char () { return Color (wrong_char_idx); }

