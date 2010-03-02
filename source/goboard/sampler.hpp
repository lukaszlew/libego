#ifndef _SAMPLER_HPP
#define _SAMPLER_HPP

#include <tr1/random>

struct Sampler {
  explicit Sampler (Board& board) : board (board), gamma ((new Gammas())) {
    ResetGammas ();
    ForEachNat (Player, pl) {
      ForEachNat (Vertex, v) {
        act_gamma [v] [pl] = 0.0;
      }
      act_gamma_sum [pl] = 0.0;
    }
  }


  ~Sampler () {
    delete gamma;
  }

  void ResetGammas () {
    ForEachNat (Player, pl) {
      ForEachNat (Hash3x3, hash) {
        (*gamma) [hash] [pl] = 
          (hash.IsLegal (pl) && !hash.IsEyelike (pl))
          ? 1.0
          : 0.0;
      }
    }
  }

  bool ReadGammas (istream& in) {
    uint raw_hash;
    double value;

    rep (ii, 2051) {
      in >> raw_hash >> value;

      if (!in) {
        ResetGammas ();
        return false;
      }
      
      Hash3x3 all[8];
      Hash3x3::OfRaw (raw_hash).GetAll8Symmetries (all);
      rep (ii, 8) {
        Hash3x3 hash = all[ii];
        if (hash.IsLegal (Player::Black()))
          (*gamma) [hash] [Player::Black()] = value;

        hash = hash.InvertColors ();
        if (hash.IsLegal (Player::White()))
          (*gamma) [hash] [Player::White()] = value;
      }
    }
    in >> raw_hash;
    if (in) {
      ResetGammas ();
      return false;
    }
    return true;
  }

  void NewPlayout () {
    // Prepare act_gamma and act_gamma_sum
    ForEachNat (Player, pl) {
      act_gamma_sum [pl] = 0.0;

      rep (ii, board.EmptyVertexCount()) {
        Vertex v = board.EmptyVertex (ii);
        act_gamma [v] [pl] = (*gamma) [board.Hash3x3At (v)] [pl];
        act_gamma_sum [pl] += act_gamma [v] [pl];
      }

      ko_v = board.KoVertex (); // TODO this assumes correct alernating play.
      act_gamma_sum [pl] -= act_gamma [ko_v] [pl];
      act_gamma [ko_v] [pl] = 0.0;
    }
  }

  // non-incremental version.
  void MovePlayed_Slow () {
    Player pl = board.ActPlayer ();

    // Prepare act_gamma and act_gamma_sum
    double sum = 0.0;

    rep (ii, board.EmptyVertexCount()) {
      Vertex v = board.EmptyVertex (ii);
      act_gamma [v] [pl] = (*gamma) [board.Hash3x3At (v)] [pl];
      sum += act_gamma [v] [pl];
    }

    ko_v = board.KoVertex ();
    sum -= act_gamma [ko_v] [pl];
    act_gamma [ko_v] [pl] = 0.0;

    act_gamma_sum [pl] = sum;
  }


  void MovePlayed () {
    Player last_pl = board.LastPlayer();
    Vertex last_v  = board.LastVertex ();

    // Restore gamma after ko_ban lifted
    ASSERT (act_gamma [ko_v] [last_pl] == 0.0);
    act_gamma [ko_v] [last_pl] = (*gamma) [board.Hash3x3At (ko_v)] [last_pl];
    act_gamma_sum [last_pl] += act_gamma [ko_v] [last_pl];

    ForEachNat (Player, pl) {
      // One new occupied intersection.
      ASSERT (board.ColorAt(last_v) != Color::Empty());
      act_gamma_sum [pl] -= act_gamma [last_v] [pl];
      act_gamma [last_v] [pl] = 0.0;

      // All new gammas.
      uint n = board.Hash3x3ChangedCount ();
      rep (ii, n) {
        Vertex v = board.Hash3x3Changed (ii);
        ASSERT (board.ColorAt(v) == Color::Empty());

        act_gamma_sum [pl] -= act_gamma [v] [pl];
        act_gamma [v] [pl] = (*gamma) [board.Hash3x3At (v)] [pl];
        act_gamma_sum [pl] += act_gamma [v] [pl];
      }
    }

    // New illegal ko point.
    Player act_pl  = board.ActPlayer();
    ko_v = board.KoVertex();
    ASSERT (board.ColorAt(ko_v) == Color::Empty() || ko_v == Vertex::Any ());
    act_gamma_sum [act_pl] -= act_gamma [ko_v] [act_pl];
    act_gamma [ko_v] [act_pl] = 0.0;
  }


  Vertex SampleMove () {
    Player pl = board.ActPlayer ();

    // Select move based on act_gamma and act_gamma_sum
    double sample = drand48() * act_gamma_sum [pl];
    ASSERT (sample < act_gamma_sum [pl] || act_gamma_sum [pl] == 0.0);
    
    double sum = 0.0;
    rep (ii, board.EmptyVertexCount()) {
      Vertex v = board.EmptyVertex (ii);
      sum += act_gamma [v] [pl];
      if (sum > sample) {
        ASSERT (act_gamma_sum [pl] > 0.0);
        return v;
      }
    }

    return Vertex::Pass();
  }


  typedef NatMap<Hash3x3, NatMap<Player, double> > Gammas;

  Board& board;
  Gammas* gamma;

  // The invariant is that act_gamma[v] is correct for all empty 
  // vertices except KoVertex() where it is 0.0.
  // act_gamma_sum is a sum of the above.
  NatMap <Vertex, NatMap<Player, double> > act_gamma;
  NatMap <Player, double> act_gamma_sum;

  Vertex ko_v;

  const static bool kCheckAsserts = false;
};

#endif
