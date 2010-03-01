// TODO player
// TODO update positive, negatice in gradient descent
// TODO mini batches

#include <vector>
using namespace std;

namespace Mm {

// -----------------------------------------------------------------------------

enum Feature {
  kPatternFeature = 0,
  feature_count = 1
};

const uint level_count [feature_count] = { 2051 };


// -----------------------------------------------------------------------------

struct Gammas {
  Gammas () {
    gammas.resize (feature_count);
    rep (feature, feature_count) {
      gammas [feature].resize (level_count [feature]);
    }
    Reset ();
  }

  void Reset () {
    rep (feature , feature_count) {
      rep (level , level_count [feature]) {
        Set (feature, level, 1.0);
      }
    }
  }

  double Get (uint feature, uint level) const {
    ASSERT (feature < feature_count);
    ASSERT (level < level_count [feature]);
    return gammas [feature] [level];
  }

  double& GetRef (uint feature, uint level) {
    ASSERT (feature < feature_count);
    ASSERT (level < level_count [feature]);
    return gammas [feature] [level];
  }

  void Set (uint feature, uint level, double value) {
    ASSERT (feature < feature_count);
    ASSERT (level < level_count [feature]);
    gammas [feature] [level] = value;
  }

  void Normalize () {
    rep (feature, feature_count) {
      double log_sum = 1.0;
      double n = 0.0;
      rep (level, level_count [feature]) {
        log_sum += log (Get (feature, level));
        n += 1.0;
      }

      double mul = exp (log_sum / n);

      rep (level, level_count [feature]) {
        gammas [feature] [level] /= mul;
      }
    }
  }

  double Distance (const Gammas& other) {
    double sum = 0.0;

    rep (feature , feature_count) {
      rep (level , level_count [feature]) {
        double d = log (Get (feature, level) / other.Get(feature, level));
        sum += d*d;
      }
    }

    return sqrt(sum);
  }

  string ToString () {
    ostringstream out;
    rep (feature , feature_count) {
      rep (level , level_count [feature]) {
        out << setprecision(5) << setw (8) << log (Get (feature, level)) << " ";
      }
      out << " | ";
    }
    return out.str ();
  }


private:
  vector <vector <double> > gammas;
};

// -----------------------------------------------------------------------------

struct Team {
  Team () {
    rep (feature, feature_count) {
      levels [feature] = 0;
    }
  }

  void SetFeatureLevel (uint feature, uint level) {
    CHECK (feature < feature_count);
    CHECK (level < level_count [feature]);
    levels [feature] = level;
  }

  double GammaExponent (uint feature, uint level) {
    return levels [feature] == level ? 1.0 : 0.0;
  }

  double TeamGamma (const Gammas& gammas) {
    double mul = 1.0;
    rep (feature, feature_count) {
      mul *= gammas.Get (feature, levels [feature]);
    }
    return mul;
  }

  double TeamGammaDiff (const Gammas& gammas, uint feature, uint level) {
    if (levels [feature] != level)
      return 0.0;

    return
      TeamGamma (gammas) / gammas.Get (feature, levels [feature]);
  }

  void GradientUpdate (Gammas& gammas, double update) {
    rep (feature, feature_count) {
      gammas.GetRef (feature, levels [feature]) *= update;
    }
  }

  string ToString () {
    ostringstream out;
    rep (ii, feature_count) {
      out << (ii == 0 ? "" : " ") << levels[ii];
    }
    return out.str();
  }

  uint levels [feature_count];
};

// -----------------------------------------------------------------------------

struct Match {

  Team& NewTeam () {
    teams.resize (teams.size() + 1);
    return teams.back();
  }

  void SetWinnerLastTeam () {
    CHECK (teams.size() > 0);
    this->winner = teams.size()-1;
  }

  void SetRandomWinner (const Gammas& gammas) {
    vector <double> team_gammas (teams.size());
    double sum = 0.0;
    rep (ii, teams.size()) {
      team_gammas [ii] = teams [ii].TeamGamma (gammas);
      sum += team_gammas [ii];
    }
    double sample = drand48() * sum;
    double sum2 = 0.0;
    rep (ii, teams.size()) {
      sum2 += team_gammas [ii];
      if (sum2 >= sample) {
        winner = ii;
        break;
      }
    }
    //cerr << "X " << ToString () << " " << sum << " " << sample << endl;
  }

  double WinnerGammaExponent (uint feature, uint level) {
    return teams [winner].GammaExponent (feature, level);
  }

  double TotalGammaDiff (const Gammas& gammas, uint feature, uint level) {
    double sum = 0.0;
    rep (ii, teams.size()) {
      sum += teams[ii].TeamGammaDiff (gammas, feature, level);
    }
    return sum;
  }

  double TotalGamma (const Gammas& gammas) {
    double sum = 0.0;
    rep (ii, teams.size()) {
      sum += teams[ii].TeamGamma (gammas);
    }
    return sum;
  }

  string ToString () {
    ostringstream out;
    rep (ii, teams.size()) {
      out << "(" << teams[ii].ToString () << ")" << (winner == ii ? "! " : "  ");
    }
    return out.str ();
  }

  void GradientUpdate (Gammas& gammas, double alpha) {
    double p [teams.size()];
    double sum = 0.0;
    rep (ii, teams.size()) {
      p [ii] = teams[ii].TeamGamma(gammas);
      sum += p[ii];
    }
    rep (ii, teams.size()) {
      p[ii] /= sum;
    }

    teams[winner].GradientUpdate (gammas, exp (alpha)); // update +alpha
    rep (ii, teams.size()) {
      teams[ii].GradientUpdate (gammas, exp (-alpha*p[ii]));
    }
  }

  vector <Team> teams;
  uint winner;
};

// -----------------------------------------------------------------------------

struct BtModel {
  BtModel () : random(123) {
    act_match = 0;
  }

  Match& NewMatch () {
    matches.resize (matches.size() + 1);
    return matches.back();
  }

  // Minorization - Maximization algorithm
  void UpdateGamma (uint feature, uint level) {
    double w = 0.0;
    double c_e = 0.0;
    rep (ii, matches.size()) {
      w   += matches [ii].WinnerGammaExponent (feature, level); // TODO precompute
      c_e +=
        matches [ii].TotalGammaDiff (gammas, feature, level) /
        matches [ii].TotalGamma (gammas);
    }

    gammas.Set (feature, level, w / c_e);
  }

  void DoFullUpdate () {
    rep (feature , feature_count) {
      rep (level , level_count [feature]) {
        UpdateGamma (feature, level);
      }
    }
    gammas.Normalize (); 
  }

  void DoGradientUpdate (uint n, double alpha) {

    rep (ii, n) {
      matches [act_match].GradientUpdate (gammas, alpha);
      act_match += 1;
      if (act_match >= matches.size()) act_match = 0;
    }
    gammas.Normalize ();
  }

  vector <Match> matches;
  uint act_match;
  Gammas gammas;
  FastRandom random;
};

// -----------------------------------------------------------------------------

void Test () {
  Gammas true_gammas;

  FastRandom rand (123);
  srand48 (123);

  rep (feature, feature_count) {
    rep (level, level_count[feature]) {
      true_gammas.Set (feature, level, exp (1 *  drand48 ()));
    }
  }
  
  true_gammas.Normalize (); 

  BtModel model;
  rep (ii, 100000) {
    Match& match = model.NewMatch ();
    rep (jj, 40) { // TODO randomize team number
      Team& team = match.NewTeam ();
      rep (feature, feature_count) {
        uint level = rand.GetNextUint (level_count[feature]);
        team.SetFeatureLevel (feature, level);
      }
    }
    match.SetRandomWinner (true_gammas);
    //cerr << ii << ": " << match.ToString () << endl;
  }

  const double a0  = 0.04;
  const double a20 = 0.006;
  const double aa = (a0 / a20 - 1.0) / 20;

  model.gammas.Reset ();

  rep (epoch, 40) {
    cerr << true_gammas.Distance (model.gammas) << " pre" <<endl;
    model.DoGradientUpdate (25000, a0 / (aa*epoch+1));
    cerr << true_gammas.Distance (model.gammas) << " a1" << endl;
    model.DoGradientUpdate (25000, a0 / (aa*epoch+1));
    cerr << true_gammas.Distance (model.gammas) << " a2" << endl << endl;
  }
  cerr << endl;

  model.gammas.Reset ();
  rep (epoch, 10) {
    cerr << true_gammas.Distance (model.gammas) << endl;
    model.DoFullUpdate ();
  }
}

// -----------------------------------------------------------------------------

} // namespace
