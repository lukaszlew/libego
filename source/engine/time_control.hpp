//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef TIME_CONTROL_H
#define TIME_CONTROL_H

#include "utils.hpp"
#include "player.hpp"
#include "gtp.hpp"

class TimeControl {
public:
  TimeControl ();
  uint PlayoutCount (Player player);
  void GtpTimeLeft (Gtp::Io& io);

  NatMap <Player, float> time_left;
  NatMap <Player, int>   time_stones;
  float playouts_per_second;
};

#endif /* TIME_CONTROL_H */
