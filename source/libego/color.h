#ifndef _COLOR_H_
#define _COLOR_H_

#include "player.h"

class Color {
public:

  static Color black ();
  static Color white ();
  static Color empty ();
  static Color off_board  ();
  static Color wrong_char ();

  explicit Color (Player pl);

  bool is_player     () const;
  bool is_not_player () const;

  Player to_player () const;

  char to_char () const;

  bool operator== (Color other) const;
  bool operator!= (Color other) const;

  bool in_range () const;
  void next ();

  explicit Color (); // TODO test - remove it
  explicit Color (uint idx_); // TODO test - remove it
  explicit Color (char c);

  const static uint black_idx = 0;
  const static uint white_idx = 1;
  const static uint empty_idx = 2;
  const static uint off_board_idx  = 3;
  const static uint wrong_char_idx = 40;
  const static uint kBound = 4;
  uint get_idx () const;

private:

  void check () const;

  uint idx;

};


// TODO test it for performance
#define color_for_each(col) \
  for (Color col = Color::black (); col.in_range (); col.next ())


#endif
