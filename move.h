#ifndef _MOVE_H_
#define _MOVE_H_

class Move {
public:
  explicit Move (Player player, Vertex v);
  explicit Move ();
  explicit Move (int idx_);

  inline Player get_player ();
  inline Vertex get_vertex ();

  string to_string ();

  inline bool operator!= (Move other) const;
  inline bool operator== (Move other) const;

  inline void next ();
  inline bool in_range () const;

  const static uint cnt = Player::white_idx << Vertex::bits_used | Vertex::cnt;
  const static uint no_move_idx = 1;

  inline uint get_idx ();

private:
  void check ();

  uint idx;
};

istream& operator>> (istream& in, Move& m);

#define move_for_each_all(m) for (Move m = Move (0); m.in_range (); m.next ())

#endif
