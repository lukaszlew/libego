#ifndef _MOVE_H_
#define _MOVE_H_

#include <string>
using namespace std;

#include "player.h"
#include "vertex.h"

class Move {
public:
  explicit Move (Player player, Vertex v);
  explicit Move ();
  explicit Move (int idx_);

  Player get_player ();
  Vertex get_vertex ();

  string to_string ();

  bool operator!= (Move other) const;
  bool operator== (Move other) const;

  void next ();
  bool in_range () const;

  const static uint cnt = Player::white_idx << Vertex::bits_used | Vertex::cnt;
  const static uint no_move_idx = 1;

  uint get_idx ();

private:
  void check ();

  uint idx;
};

istream& operator>> (istream& in, Move& m);

#define move_for_each_all(m) for (Move m = Move (0); m.in_range (); m.next ())

#endif
