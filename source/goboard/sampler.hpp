#ifndef _SAMPLER_HPP
#define _SAMPLER_HPP

#include <tr1/random>

struct Sampler {
  explicit Sampler (Board& board) : board (board), gamma ((new Gammas())) {
    ForEachNat (Player, pl) {
      ForEachNat (Hash3x3, hash) {
        (*gamma) [pl] [hash] = 
          (hash.IsLegal (pl) && !hash.IsEyelike (pl))
          ? 1.0
          : 0.0;
      }
    }
  }


  ~Sampler () {
    delete gamma;
  }


  Vertex SampleMove () {
    Player pl = board.ActPlayer ();
    double sum;

    // Prepare act_gamma and act_gamma_sum
    sum = 0.0;
    rep (ii, board.EmptyVertexCount()) {
      Vertex v = board.EmptyVertex (ii);
      act_gamma [v] = (*gamma) [pl] [board.Hash3x3At (v)];
      sum += act_gamma[v];
    }
    {
      // TODO ko is always last in EmptyVertex seq.
      Vertex ko_v = board.KoVertex ();
      if (ko_v != Vertex::Any ()) {
        sum -= act_gamma [ko_v];
        act_gamma [ko_v] = 0.0;
      }
    }
    act_gamma_sum = sum;

    // Select move based on act_gamma and act_gamma_sum
    double sample = drand48() * act_gamma_sum;
    ASSERT (sample < act_gamma_sum || act_gamma_sum == 0.0);
    
    sum = 0.0;
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


  typedef NatMap<Player, NatMap<Hash3x3, double> > Gammas;

  Board& board;
  Gammas* gamma;
  double act_gamma_sum;

  NatMap <Vertex, double> act_gamma;

  const static bool kCheckAsserts = false;
};

#endif
