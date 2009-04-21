#ifndef _STAT_H_
#define _STAT_H_

class Stat {
public:
  Stat () {
    reset ();
  }
  
  void reset (float prior_sample_count = 1.0) {
    sample_count       = prior_sample_count; // TODO 
    sample_sum         = 0.0; // TODO
    square_sample_sum  = 0.0; // TODO
  }

  void update (float sample) {
    sample_count       += 1.0;
    sample_sum         += sample;
    square_sample_sum  += sample * sample;
  }

  float update_count () {
    return sample_count;
  }

  float mean () { 
    return sample_sum / sample_count; 
  }

  float variance () {
    // VX = E(X^2) - EX ^ 2
    float m = mean ();
    return square_sample_sum / sample_count - m * m;
  }

  float std_dev () { 
    return sqrt (variance ());
  }

  float std_err () {
    // TODO assert sample_count
    return sqrt (variance () / sample_count);
  } 

  float ucb (Player pl, float explore_coeff) {
    return 
      (pl == Player::black () ? mean() : -mean()) +
      sqrt (explore_coeff / update_count());
  }

  string to_string (bool dont_show_unupdated = true) {
    if (dont_show_unupdated && sample_count < 2.0) return "           ";

    ostringstream out;
    char buf [100];
    sprintf (buf, "%+3.1f(%5.0f)", mean (), sample_count);
    out << buf;
    return out.str ();
  }

private:
  float sample_count;
  float sample_sum;
  float square_sample_sum;
};

#endif
