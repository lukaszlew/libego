//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "color.h"
#include "testing.h"

Color::Color () : Nat<Color> () {
}

Color::Color (uint raw) : Nat<Color> (raw) {
}

Color Color::Black() {
  return Color (0);
}

Color Color::White() {
  return Color (1);
}

Color Color::Empty() {
  return Color (2);
}

Color Color::OffBoard() {
  return Color (3);
}

Color Color::OfPlayer (Player pl) {
  return OfRaw (pl.GetRaw ());
}


bool Color::IsPlayer () const {
  return GetRaw() <= 1;
} 

bool Color::IsNotPlayer () const {
  return GetRaw() > 1;
}

Player Color::ToPlayer () const {
  return Player::OfRaw (GetRaw());
}

char Color::ToShowboardChar () const { 
  NatMap <Color, char> tab;
  tab [Black()] = '#';
  tab [White()] = 'O';
  tab [Empty()] = '.';
  tab [OffBoard()] = '$';
  return tab [*this];
}
