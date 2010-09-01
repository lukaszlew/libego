#ifndef _GAMMAS_HPP
#define _GAMMAS_HPP

#include "hash.hpp"

const double GammaskAccurancy = 1.0e-10;



class Gammas {
public:
  Gammas () {
    gammas = new Tab;
    ResetToUniform ();
  }

  ~Gammas () {
    delete gammas;
  }

  void ZeroAllGammas () {
    ForEachNat (Hash3x3, hash) {
      ForEachNat (Player, pl) {
        (*gammas) [hash] [pl] = 0.0;
      }
    }
  }


  void ResetToUniform () {
    ForEachNat (Hash3x3, hash) {
      ForEachNat (Player, pl) {
        (*gammas) [hash] [pl] = 
          (hash.IsLegal (pl) && !hash.IsEyelike (pl))
          ? 1.0
          : 0.0;
      }
    }
  }


  bool Read (istream& in) {
    uint raw_hash;
    double value;
    string c;

    ZeroAllGammas ();

    rep (ii, 2051) {
      in >> raw_hash >> c >> value;
      
      if (!in || c != ",") {
        ResetToUniform ();
        cerr << "Error at:" << ii << endl;
        return false;
      }
      
      Hash3x3 all[8];
      Hash3x3::OfRaw (raw_hash).GetAll8Symmetries (all);
      rep (ii, 8) {
        Hash3x3 hash = all[ii];
        CHECK (value > 0.0);
        CHECK (hash.IsLegal (Player::Black ()));
        CHECK (value > GammaskAccurancy * 100);

        // Note: We zero values of play-in-eye
        if (!hash.IsEyelike (Player::Black())) {
          (*gammas) [hash] [Player::Black()] = value;
          hash = hash.InvertColors ();
          (*gammas) [hash] [Player::White()] = value;
        }
      }
    }
    // check that nothing more can be read
    in >> raw_hash;
    if (in) {
      ResetToUniform ();
      return false;
    }
    return true;
  }


  double Get (Hash3x3 hash, Player pl) const {
    return (*gammas) [hash] [pl];
  }

private:

  typedef NatMap<Hash3x3, NatMap<Player, double> > Tab;
  Tab* gammas;
};

#endif

