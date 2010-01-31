//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef COLOR_H_
#define COLOR_H_

#include "player.hpp"

class Color : public Nat <Color> {
public:

  // Constructors.

  explicit Color();

  static Color Black();
  static Color White();
  static Color Empty();
  static Color OffBoard();

  static Color OfPlayer (Player pl);

  // Utilities.

  bool IsPlayer() const;
  bool IsNotPlayer() const;
  Player ToPlayer() const;
  char ToShowboardChar() const;

  // Other.

  static const uint kBound = 4;

private:
  friend class Nat <Color>;
  explicit Color (uint raw);
};

#endif
