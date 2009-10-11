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

  static float Mix (const Stat& s1, float b1, const Stat& s2, float b2) {
    float n_1    = s1.update_count ();
    float mean_1 = s1.mean();
    float var_1  = s1.variance();
    float prec_1 = 1 / (var_1 / n_1 + b1);

    float n_2    = s2.update_count ();
    float mean_2 = s2.mean();
    float var_2  = s2.variance();
    float prec_2 = 1 / (var_2 / n_2 + b2);

    return (mean_1 * prec_1 + mean_2 * prec_2) / (prec_1 + prec_2);
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
