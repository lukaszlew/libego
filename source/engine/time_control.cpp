//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "time_control.hpp"
#include "param.hpp"
#include "gtp_gogui.hpp"

extern Gtp::ReplWithGogui gtp;


TimeControl::TimeControl () :
    time_left (0.0),
    time_stones (-1),
    playouts_per_second (10000)
  {
    gtp.Register ("time_left", this, &TimeControl::GtpTimeLeft);
  }

  uint TimeControl::PlayoutCount (Player player) {
    int playouts = Param::genmove_playouts;
    if (time_stones [player] == 0 && time_left [player] < 60.0) {
      playouts = min (playouts,
                      int (time_left [player] / 30.0 * playouts_per_second));
      playouts = max (playouts, 1000);
    }
    return playouts;
  }

  void TimeControl::GtpTimeLeft (Gtp::Io& io) {
    Player pl = io.Read<Player> ();
    float seconds = io.Read<float> ();
    int stones = io.Read<int> ();
    time_left [pl] = seconds;
    time_stones [pl] = stones;
  }
