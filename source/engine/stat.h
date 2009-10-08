#ifndef _STAT_H_
#define _STAT_H_

class Stat {
public:

  static const float prior_update_count = 1.0;

  Stat () {
    reset ();
  }
  
  void reset (float prior_sample_count = Stat::prior_update_count) {
    sample_count       = prior_sample_count; // TODO 
    sample_sum         = 0.0; // TODO
    square_sample_sum  = 0.0; // TODO
  }

  void update (float sample) {
    sample_count       += 1.0;
    sample_sum         += sample;
    square_sample_sum  += sample * sample;
  }

  float update_count () const {
    return sample_count;
  }

  float mean () const { 
    return sample_sum / sample_count; 
  }

  float variance () const {
    // VX = E(X^2) - EX ^ 2
    float m = mean ();
    return square_sample_sum / sample_count - m * m;
  }

  float std_dev () const { 
    return sqrt (variance ());
  }

  float std_err () const {
    // TODO assert sample_count
    return sqrt (variance () / sample_count);
  } 

  float ucb (Player pl, float explore_coeff) const {
    return 
      (pl == Player::black () ? mean() : -mean()) +
      sqrt (explore_coeff / update_count());
  }

  string to_string (float minimal_update_count = 0.0) const {
    if (sample_count < minimal_update_count) return "           ";

    ostringstream out;
    char buf [100];
    sprintf (buf, "%+3.3f(%5.0f)", mean(), update_count());
    out << buf;
    return out.str ();
  }

private:
  float sample_count;
  float sample_sum;
  float square_sample_sum;
};

#endif
