
class Mcmc {
public:
  Mcmc () {
    Reset ();
  }

  void Reset () {
    // TODO prior randomization here!
    ForEachNat (Move, m2) {
      ForEachNat (Move, m1) {
        ForEachNat (Move, m0) {
          ms3[m2][m1][m0].reset (1.0, 0.0);
        }
      }
    }
  }

  void NewPlayout () {
  }

  void Update (float score, Move pre, const vector<Move>& history) {
    if (history.size() < 1) return;
    Move m2 = pre;
    Move m1 = history[0];

    NatMap<Vertex, uint> play_count; // TODO unify RAVE; set 1 on nonempty
    play_count.SetToZero ();
    play_count [m2.GetVertex()] += 1;
    play_count [m1.GetVertex()] += 1;
    play_count [Vertex::Any()] = 1; // beginning of the game is Any*2

    reps (ii, 1, history.size() * Param::mcmc_update_fraction) {
      Move m0 = history[ii];
      play_count [m0.GetVertex()] += 1;
      if (play_count [m2.GetVertex()] == 1 &&
          play_count [m1.GetVertex()] == 1 &&
          play_count [m0.GetVertex()] == 1)
      {
        Stat& s = ms3 [m2] [m1] [m0];
        s.update (score);
        s.UpdateUcb (m0.GetPlayer (), Param::mcmc_explore_coeff);
      }
      m2 = m1;
      m1 = m0;
    }
  }

  Vertex Choose8Move (const Board& board, const NatMap<Vertex, uint>& play_count) {
    Move m2 = board.LastMove2 ();
    Move m1 = board.LastMove ();

    Vertex last_v = board.LastVertex();

    //if (last_v == Vertex::Any ())  return Vertex::Any ();
    //if (last_v == Vertex::Pass ()) return Vertex::Any ();
    if (play_count [m2.GetVertex ()] > 1) return Vertex::Any ();
    if (play_count [m1.GetVertex ()] > 1) return Vertex::Any ();

    Vertex best_v = Vertex::Any(); // any == light move
    float best_value = - 1E20;
    MS1& my_ms1 = ms3 [m2] [m1];
    Player pl = board.ActPlayer();
        
    ForEachNat (Vertex, nbr) {
      if (play_count[nbr] == 0 &&
          board.IsLegal (pl, nbr) &&
          !board.IsEyelike (pl, nbr))
      {
        Stat& stat = my_ms1 [Move(pl, nbr)];
        float value = stat.Ucb();
        if (best_value < value) {
          best_value = value;
          best_v = nbr;
        }
      }
    }

    return best_v;
  }

  void MoveValueGfx (const Board& board, Gtp::GoguiGfx* gfx)
  {
    Stat stat(0.0, 0.0);
    stat.update (-1);
    stat.update (1);

    MS1& my_ms1 = ms3 [board.LastMove2 ()] [board.LastMove ()];
    Player pl = board.ActPlayer ();

    ForEachNat (Vertex, v) {
      if (board.ColorAt (v) == Color::Empty ()) {
        Move m = Move (pl, v);
        if (my_ms1 [m].update_count() == 1) continue;
        float mean = my_ms1 [m].mean();
        stat.update (mean);
      }
    }
    ForEachNat (Vertex, v) {
      if (board.ColorAt (v) == Color::Empty ()) {
        Move m = Move (pl, v);
        if (my_ms1 [m].update_count() == 1) continue;
        float mean = my_ms1 [m].mean();
        float val = (mean - stat.mean()) / stat.std_dev () / 3;
        gfx->SetInfluence (v.ToGtpString (), val);
        cerr << v.ToGtpString () << " : "
             << my_ms1 [m].to_string () << endl;
      }
    }
  }

  typedef NatMap <Move, Stat> MS1;
  typedef NatMap <Move, MS1>  MS2;
  typedef NatMap <Move, MS2>  MS3;
  MS3 ms3;
};
