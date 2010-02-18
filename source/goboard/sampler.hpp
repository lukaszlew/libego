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

    sum = 0.0;
    rep (ii, board.EmptyVertexCount()) {
      Vertex v = board.EmptyVertex (ii);
      if (v == board.KoVertex ()) continue;
      act_gamma [v] = (*gamma) [pl] [board.Hash3x3At (v)];
      sum += act_gamma[v];
    }
    act_gamma_sum = sum;

    if (act_gamma_sum == 0.0) return Vertex::Pass (); // TODO gamma for pass

    double sample = drand48() * act_gamma_sum;
    ASSERT (sample < act_gamma_sum);
    
    sum = 0.0;
    rep (ii, board.EmptyVertexCount()) {
      Vertex v = board.EmptyVertex (ii);
      if (v == board.KoVertex ()) continue;
      sum += act_gamma [v];
      if (sum > sample) return v;
    }

    return Vertex::Pass();
    CHECK (false);
  }


  typedef NatMap<Player, NatMap<Hash3x3, double> > Gammas;

  Board& board;
  Gammas* gamma;
  double act_gamma_sum;

  NatMap <Vertex, double> act_gamma;

  const static bool kCheckAsserts = false;
};

#endif
