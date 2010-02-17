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

    double gamma_sum = 0.0;
    rep (ii, board.EmptyVertexCount()) {
      Vertex v = board.EmptyVertex (ii);
      if (v == board.KoVertex ()) continue;
      gamma_sum += (*gamma) [pl] [board.Hash3x3At (v)];
    }

    last_sum = gamma_sum;

    if (gamma_sum == 0.0) return Vertex::Pass (); // TODO gamma for pass

    double sample = random.NextDouble (gamma_sum);

    gamma_sum = 0.0;
    rep (ii, board.EmptyVertexCount()) {
      Vertex v = board.EmptyVertex (ii);
      if (v == board.KoVertex ()) continue;
      gamma_sum += (*gamma) [pl] [board.Hash3x3At (v)];
      if (gamma_sum >= sample) return v;
    }
    CHECK (false);
  }

  typedef NatMap<Player, NatMap<Hash3x3, double> > Gammas;

  Board& board;
  Gammas* gamma;
  Random random;
  double last_sum;
};

#endif
