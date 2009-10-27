//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "color.h"
#include "testing.h"

Color::Color () : Nat<4> () {
}

Color::Color (uint raw) : Nat<4> (raw) {
}

Color Color::Black() {
  return Color (black_idx);
}

Color Color::White() {
  return Color (white_idx);
}

Color Color::Empty() {
  return Color (empty_idx);
}

Color Color::OffBoard() {
  return Color (off_board_idx);
}

Color Color::OfRaw (uint raw) {
  return Color (raw);
}

Color Color::OfPlayer (Player pl) {
  return OfRaw (pl.GetRaw ());
}


bool Color::IsPlayer () const {
  return GetRaw() <= white_idx;      // & (~1)) == 0; }
} 

bool Color::IsNotPlayer () const {
  return GetRaw() > white_idx;      // & (~1)) == 0; }
}

Player Color::ToPlayer () const {
  // TODO assert
  return Player::OfRaw (GetRaw());
}

char Color::ToShowboardChar () const { 
  switch (GetRaw()) {
  case black_idx:      return '#';
  case white_idx:      return 'O';
  case empty_idx:      return '.';
  case off_board_idx:  return ' ';
  default : assertc (color_ac, false);
  }
  return '?';                 // should not happen
}
