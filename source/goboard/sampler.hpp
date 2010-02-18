#ifndef _SAMPLER_HPP
#define _SAMPLER_HPP

#include <tr1/random>

struct Sampler {
  explicit Sampler (Board& board) : board (board), gamma ((new Gammas())) {
    ForEachNat (Player, pl) {
      ForEachNat (Hash3x3, hash) {
        (*gamma) [hash] [pl] = 
          (hash.IsLegal (pl) && !hash.IsEyelike (pl))
          ? 1.0
          : 0.0;
      }
    }
    act_gamma [Vertex::Any()] = 0.0;
    act_gamma_sum = 0.0; // to avoid warning
  }


  ~Sampler () {
    delete gamma;
  }


  void NewPlayout () {
    Player pl = board.ActPlayer ();

    // Prepare act_gamma and act_gamma_sum
    act_gamma_sum = 0.0;

    rep (ii, board.EmptyVertexCount()) {
      Vertex v = board.EmptyVertex (ii);
      act_gamma [v] = (*gamma) [board.Hash3x3At (v)] [pl];
      act_gamma_sum += act_gamma[v];
    }

    Vertex ko_v = board.KoVertex ();
    act_gamma_sum -= act_gamma [ko_v];
    act_gamma [ko_v] = 0.0;
  }


  void MovePlayed () {
    Player pl = board.ActPlayer ();

    // Prepare act_gamma and act_gamma_sum
    double sum = 0.0;

    rep (ii, board.EmptyVertexCount()) {
      Vertex v = board.EmptyVertex (ii);
      act_gamma [v] = (*gamma) [board.Hash3x3At (v)] [pl];
      sum += act_gamma[v];
    }

    Vertex ko_v = board.KoVertex ();
    sum -= act_gamma [ko_v];
    act_gamma [ko_v] = 0.0;

    act_gamma_sum = sum;
  }


  Vertex SampleMove () {
    // Select move based on act_gamma and act_gamma_sum
    double sample = drand48() * act_gamma_sum;
    ASSERT (sample < act_gamma_sum || act_gamma_sum == 0.0);
    
    double sum = 0.0;
    rep (ii, board.EmptyVertexCount()) {
      Vertex v = board.EmptyVertex (ii);
      sum += act_gamma [v];
      if (sum > sample) {
        ASSERT (act_gamma_sum > 0.0);
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
  NatMap <Vertex, double> act_gamma;
  double act_gamma_sum;

  const static bool kCheckAsserts = false;
};

#endif
