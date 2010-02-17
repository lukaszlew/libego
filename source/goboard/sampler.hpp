#ifndef _SAMPLER_HPP
#define _SAMPLER_HPP

#include <tr1/random>

struct Random {
  std::tr1::mt19937 mt;
  double NextDouble (double max = 1.0) {
    std::tr1::uniform_real<double> uniform (max);
    return uniform (mt);
  }
};



struct Sampler {
  explicit Sampler (Board& board) : board (board) {
    ForEachNat (Player, pl) {
      ForEachNat (Hash3x3, hash) {
        gamma [pl] [hash] = hash.IsLegal (pl) ? 1.0 : 0.0;
      }
    }
  }

  Vertex SampleMove () {
    Player pl = board.ActPlayer ();

    double gamma_sum = 0.0;
    rep (ii, board.EmptyVertexCount()) {
      Vertex v = board.EmptyVertex (ii);
      if (v == board.KoVertex ()) continue;
      gamma_sum += gamma [pl] [board.Hash3x3At (v)];
    }

    last_sum = gamma_sum;

    if (gamma_sum == 0.0) return Vertex::Pass (); // TODO gamma for pass

    double sample = random.NextDouble (gamma_sum);

    gamma_sum = 0.0;
    rep (ii, board.EmptyVertexCount()) {
      Vertex v = board.EmptyVertex (ii);
      if (v == board.KoVertex ()) continue;
      gamma_sum += gamma [pl] [board.Hash3x3At (v)];
      if (gamma_sum >= sample) return v;
    }
    CHECK (false);
  }

  NatMap<Player, NatMap<Hash3x3, double> > gamma;
  Board& board;
  Random random;
  double last_sum;
};

#endif
