//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef COLOR_H_
#define COLOR_H_

#include "player.h"

class Color : public Nat<4> {
public:
  // Constructors.
  explicit Color();

  static Color Black();
  static Color White();
  static Color Empty();
  static Color OffBoard();
  static Color OfRaw (uint raw);
  static Color OfPlayer (Player pl);

  // Utilities.

  bool IsPlayer() const;
  bool IsNotPlayer () const;
  Player ToPlayer () const;
  char ToShowboardChar () const;

  const static uint black_idx = 0;
  const static uint white_idx = 1;
  const static uint empty_idx = 2;
  const static uint off_board_idx  = 3;
  const static uint wrong_char_idx = 40;

private:
  explicit Color (uint raw);
};


// TODO test it for performance
#define color_for_each(col) for (Color col; col.Next();)


#endif
