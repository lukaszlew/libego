#ifndef _PLAYER_H_
#define _PLAYER_H_

class Player { // TODO check is check always checked in constructors
public:

  static Player black ();
  static Player white ();

  const static uint black_idx = 0;
  const static uint white_idx = 1;
  const static uint cnt = 2;
  uint get_idx () const;

  Player other () const;

  string to_string () const;

  explicit Player ();
  explicit Player (uint _idx);

  bool operator== (Player other) const;
  
  // TODO iterators?
  bool in_range () const; // TODO do it like check
  void next ();

private:

  void check () const;

  uint idx;
};

#endif
