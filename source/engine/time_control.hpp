#ifndef TIME_CONTROL_H
#define TIME_CONTROL_H
//
// Copyright 2006 and onwards, Lukasz Lew
//

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
