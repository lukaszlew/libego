//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef PLAYER_INL_H_
#define PLAYER_INL_H_

template <typename T>
T Player::SubjectiveScore (const T& score) const {
  NatMap <Player, T> tab (0);
  tab[Black()] = score;
  tab[White()] = - score;
  return tab [*this];
}

#endif
