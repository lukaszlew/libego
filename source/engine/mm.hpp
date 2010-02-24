// TODO player
// TODO update positive, negatice in gradient descent
// TODO mini batches

#include <vector>
using namespace std;

namespace BradleyTerry {

// -----------------------------------------------------------------------------

const uint feature_count = 5;
  const uint level_count [feature_count] = { 5, 4, 3, 2, 3 };


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

  void Set (uint feature, uint level, double value) {
    ASSERT (feature < feature_count);
    ASSERT (level < level_count [feature]);
    gammas [feature] [level] = value;
  }

  void Normalize () {
    rep (feature, feature_count) {
      double mul = 1.0;
      double n = 0.0;
      rep (level, level_count [feature]) {
        mul *= Get (feature, level);
        n += 1.0;
      }
      mul = pow (mul, 1.0 / n);

      rep (level, level_count [feature]) {
        gammas [feature] [level] /= mul;
      }
    }
  }

  string ToString () {
    ostringstream out;
    rep (feature , feature_count) {
      rep (level , level_count [feature]) {
        out << setprecision(6) << setw (10) << Get (feature, level) << " ";
      }
      out << " | ";
    }
    return out.str ();
  }


private:
  vector <vector <double> > gammas;
};

// -----------------------------------------------------------------------------

Gammas gammas;

struct Team {
  Team () {
    levels.resize (feature_count, 0);
  }

  void SetFeatureLevel (uint feature, uint level) {
    CHECK (feature < feature_count);
    CHECK (level < level_count [feature]);
    levels [feature] = level;
  }

  double GammaExponent (uint feature, uint level) {
    return levels [feature] == level ? 1.0 : 0.0;
  }

  double TeamGamma () {
    double mul = 1.0;
    rep (feature, feature_count) {
      mul *= gammas.Get (feature, levels [feature]);
    }
    return mul;
  }

  double TeamGammaDiff (uint feature, uint level) {
    if (levels [feature] != level)
      return 0.0;

    return
      TeamGamma () / gammas.Get (feature, levels [feature]);
  }

  string ToString () {
    ostringstream out;
    rep (ii, levels.size()) {
      out << (ii == 0 ? "" : " ") << levels[ii];
    }
    return out.str();
  }

  vector <uint> levels;
};

// -----------------------------------------------------------------------------

struct Match {

  Team& NewTeam () {
    teams.resize (teams.size() + 1);
    return teams.back();
  }

  void SetWinner (uint winner) {
    CHECK (winner < teams.size());
    this->winner = winner;
  }

  void SetRandomWinner () {
    vector <double> team_gammas (teams.size());
    double sum = 0.0;
    rep (ii, teams.size()) {
      team_gammas [ii] = teams [ii].TeamGamma ();
      sum += team_gammas [ii];
    }
    double sample = drand48() * sum;
    double sum2 = 0.0;
    rep (ii, teams.size()) {
      sum2 += team_gammas [ii];
      if (sum2 >= sample) {
        SetWinner (ii);
        break;
      }
    }
    //cerr << "X " << ToString () << " " << sum << " " << sample << endl;
  }

  double WinnerGammaExponent (uint feature, uint level) {
    return teams [winner].GammaExponent (feature, level);
  }

  double TotalGammaDiff (uint feature, uint level) {
    double sum = 0.0;
    rep (ii, teams.size()) {
      sum += teams[ii].TeamGammaDiff (feature, level);
    }
    return sum;
  }

  double TotalGamma () {
    double sum = 0.0;
    rep (ii, teams.size()) {
      sum += teams[ii].TeamGamma ();
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

  vector <Team> teams;
  uint winner;
};

// -----------------------------------------------------------------------------

struct BtModel {

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
        matches [ii].TotalGammaDiff (feature, level) /
        matches [ii].TotalGamma ();
    }

    gammas.Set (feature, level, w / c_e);
  }

  void DoFullUpdate () {
    rep (feature , feature_count) {
      rep (level , level_count [feature]) {
        UpdateGamma (feature, level);
      }
    }
  }

  vector <Match> matches;
};

// -----------------------------------------------------------------------------

void Test () {
  Gammas true_gammas;

  FastRandom rand (123);
  srand48 (123);

  rep (feature, feature_count) {
    rep (level, level_count[feature]) {
      true_gammas.Set (feature, level, exp (3 * drand48 ()));
    }
  }
  
  true_gammas.Normalize (); 

  // just for setting the random winner
  gammas = true_gammas;

  BtModel model;
  rep (ii, 100000) {
    Match& match = model.NewMatch ();
    rep (jj, 3) { // TODO randomize team number
      Team& team = match.NewTeam ();
      rep (feature, feature_count) {
        uint level = rand.GetNextUint (level_count[feature]);
        team.SetFeatureLevel (feature, level);
      }
    }
    match.SetRandomWinner ();
    //cerr << ii << ": " << match.ToString () << endl;
  }

  gammas.Reset ();

  rep (epoch, 100) {
    model.DoFullUpdate ();
    gammas.Normalize (); 
    WW (epoch);
    cerr << true_gammas.ToString () << endl;
    cerr << gammas.ToString () << endl << endl;
  }

}

// -----------------------------------------------------------------------------

} // namespace
