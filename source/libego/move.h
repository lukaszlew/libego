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

  Move other_player ();

  string to_string ();

  bool operator!= (Move other) const;
  bool operator== (Move other) const;

  void next ();
  bool in_range () const;

  const static uint kBound = Player::white_idx << Vertex::bits_used | Vertex::kBound;
  const static uint no_move_idx = 1;

  uint GetRaw ();

  bool TryInc () {
    if (idx+1 < kBound) {
      idx += 1;
      return true;
    }
    return false;
  }
private:
  void check ();

  uint idx;
};

istream& operator>> (istream& in, Move& m);

#define move_for_each_all(m) for (Move m = Move (0); m.in_range (); m.next ())

#endif
