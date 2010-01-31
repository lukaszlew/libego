//
// Copyright 2006 and onwards, Lukasz Lew
//

class TimeControl {
public:
  TimeControl (Gtp::ReplWithGogui& gtp) :
    time_left (0.0),
    time_stones (-1),
    playouts_per_second (10000)
  {
    gtp.Register ("time_left", this, &TimeControl::GtpTimeLeft);
  }

  uint PlayoutCount (Player player) {
    int playouts = Param::genmove_playouts;
    if (time_stones [player] == 0 && time_left [player] < 60.0) {
      playouts = min (playouts,
                      int (time_left [player] / 30.0 * playouts_per_second));
      playouts = max (playouts, 1000);
    }
    return playouts;
  }

  void GtpTimeLeft (Gtp::Io& io) {
    Player pl = io.Read<Player> ();
    float seconds = io.Read<float> ();
    int stones = io.Read<int> ();
    time_left [pl] = seconds;
    time_stones [pl] = stones;
  }

  NatMap <Player, float> time_left;
  NatMap <Player, int>   time_stones;
  float playouts_per_second;
};
