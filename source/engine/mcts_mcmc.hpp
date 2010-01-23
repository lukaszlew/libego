#define for_each_8_nbr(center_v, nbr_v, block) {                \
    Vertex nbr_v;                                               \
    nbr_v = center_v.N (); block;                               \
    nbr_v = center_v.W (); block;                               \
    nbr_v = center_v.E (); block;                               \
    nbr_v = center_v.S (); block;                               \
    nbr_v = center_v.NW (); block;                              \
    nbr_v = center_v.NE (); block;                              \
    nbr_v = center_v.SW (); block;                              \
    nbr_v = center_v.SE (); block;                              \
  }


namespace Param {
  bool mcmc_use    = true;
  bool mcmc_update = true;
  float mcmc_update_fraction = 0.5;
  float mcmc_explore_coeff = 10.0;
  float mcmc_prob_8_nbr = 0.8;
}

struct McmcNode {
  NatMap <Move, Stat> move_stats;
  //  NatMap <Player, Stat> light;
};

struct Trace {
  Move m1;
  Move m0;
};

// -----------------------------------------------------------------------------

class Mcmc {
public:
  Mcmc () {
    Reset ();
  }

  void Reset () {
    // TODO prior, pass
    // TODO prior randomization here!
    ForEachNat (Move, m1) {
      ForEachNat (Move, m0) {
        mcmc[m1].move_stats [m0].reset (1.0, 0.0);
      }
    }
    //ForEachNat (Player, pl) mcmc[m].light[pl].reset (1.0, 0.0);    
  }

  void NewPlayout () {
    to_update.clear();
    prob_8mcmc_1024 = 1024 * Param::mcmc_prob_8_nbr;
  }

  void Update (float score) {
    rep (ii, to_update.size() * Param::mcmc_update_fraction) {
      Move m0 = to_update[ii].m0;
      Move m1 = to_update[ii].m1;
      Stat& s = mcmc [m1].move_stats [m0];
      s.update (score);
      s.UpdateUcb (m0.GetPlayer (), Param::mcmc_explore_coeff);
    }
  }

  Vertex Choose8Move (const Board& board, const NatMap<Vertex, uint>& play_count) {
    Vertex last_v = board.LastVertex();

    if (last_v == Vertex::Any ())  return Vertex::Any ();
    if (last_v == Vertex::Pass ()) return Vertex::Any ();
    if (play_count [last_v] != 1)  return Vertex::Any ();

    Vertex best_v = Vertex::Any(); // any == light move
    float best_value = - 1E20;
    McmcNode& act_mcmc = mcmc [board.LastMove ()];
    Player pl = board.ActPlayer();
        
    // TODO to jest za sztywne, 8 sasiadow
    for_each_8_nbr (last_v, nbr, {
      if (play_count[nbr] == 0 &&
          board.IsLegal (pl, nbr) &&
          !board.IsEyelike (pl, nbr))
      {
        Stat& stat = act_mcmc.move_stats [Move(pl, nbr)];
        float value = stat.Ucb(); // TODO UCB?
        if (best_value < value) {
          best_value = value;
          best_v = nbr;
        }
      }
    });

    return best_v;
  }

  void MovePlayed (Move m1, Move m0, const NatMap<Vertex, uint>& play_count) {
    if (play_count [m1.GetVertex ()] != 1) return;
    if (play_count [m0.GetVertex ()] != 1) return;
    Trace t;
    t.m1 = m1;
    t.m0 = m0;
    to_update.push_back (t);
  }
  
  void MoveProbGfx (Move pre_move,
                    Player player,
                    const Board& board,
                    Gtp::GoguiGfx* gfx)
  {
    unused (pre_move);
    unused (player);
    unused (board);
    unused (gfx);
  }

  void MoveValueGfx (Move pre_move,
                     Player player,
                     const Board& board,
                     Gtp::GoguiGfx* gfx)
  {
    Stat stat(0.01, 0.0);
    Vertex pre_v = pre_move.GetVertex();

    for_each_8_nbr (pre_v, v, {
      if (board.ColorAt (v) == Color::Empty () && v != pre_move.GetVertex()) {
        Move move = Move (player, v);
        float mean = mcmc [pre_move].move_stats [move].mean();
        stat.update (mean);
      }
    });

//     cerr << "light" << " : "
//          << mcmc [pre_move].light [player].to_string () << endl;

    for_each_8_nbr (pre_v, v, {
        //ForEachNat (Vertex, v) {
      if (board.ColorAt (v) == Color::Empty () &&
          v != pre_move.GetVertex()) {
        Move move = Move (player, v);
        float mean = mcmc [pre_move].move_stats [move].mean();
        float val = (mean - stat.mean()) / stat.std_dev () / 3;
        gfx->SetInfluence (v.ToGtpString (), val);
        cerr << v.ToGtpString () << " : "
             << mcmc [pre_move].move_stats [move].to_string () << endl;
      }
    });
  }

  NatMap <Move, McmcNode> mcmc;
  vector <Trace> to_update;
  uint prob_8mcmc_1024;
};
// -----------------------------------------------------------------------------

