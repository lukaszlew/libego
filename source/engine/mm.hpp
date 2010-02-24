// TODO player
// TODO update positive, negatice in gradient descent
// TODO mini batches

#include <vector>
using namespace std;

// -----------------------------------------------------------------------------

const uint feature_count = 3;
const uint level_count [feature_count] = { 2, 3, 4 };


struct GammaId {
  uint feature;
  uint level;
};


struct Gammas {
  Gammas () {
    CHECK (false); //TODO
  }

  double& operator[] (const GammaId& gamma_id) {
    return gammas [gamma_id.feature] [gamma_id.level];
  }

  vector <vector <double> > gammas;
};

// -----------------------------------------------------------------------------

Gammas gammas;

struct Team {
  
  double GammaExponent (const GammaId& gamma_id) {
    return levels [gamma_id.feature] == gamma_id.level ? 1.0 : 0.0;
  }

  double TeamGamma () {
    double mul = 1.0;
    rep (feature, feature_count) {
      mul *= gammas.gammas [feature] [levels [feature]];
    }
    return mul;
  }

  double TeamGammaDiff (const GammaId& gamma_id) {
    if (levels [gamma_id.feature] != gamma_id.level)
      return 0.0;

    return
      TeamGamma () / gammas.gammas [gamma_id.feature] [levels [gamma_id.feature]];
  }

  vector <uint> levels;
};

// -----------------------------------------------------------------------------

struct Match {
  double WinnerGammaExponent (const GammaId& gamma_id) {
    return teams [winner].GammaExponent (gamma_id);
  }

  double TotalGammaDiff (const GammaId& gamma_id) {
    double sum = 0.0;
    rep (ii, teams.size()) {
      sum += teams[ii].TeamGammaDiff (gamma_id);
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

  vector <Team> teams;
  uint winner;
};

// -----------------------------------------------------------------------------

struct BtModel {

  void MmUpdate (const GammaId& gamma_id) {
    double w = 0.0;
    double c_e = 0.0; // 
    rep (ii, matches.size()) {
      w   += matches [ii].WinnerGammaExponent (gamma_id); // TODO precompute
      c_e +=
        matches [ii].TotalGammaDiff (gamma_id) /
        matches [ii].TotalGamma ();
    }

    gammas [gamma_id] = w / c_e;
  }

  vector <Match> matches;
};

// -----------------------------------------------------------------------------

/*



*/
