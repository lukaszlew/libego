#ifndef ENGINE_H_
#define ENGINE_H_
//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "logger.hpp"
#include "to_string.hpp"
#include "gtp_gogui.hpp"
#include "ego.hpp"
#include "time_control.hpp"
#include "mcts_tree.hpp"

class Engine {
public:
  
  Engine () :
    random (TimeSeed()),
    sampler (play_board, gammas)
  {
  }

  bool SetBoardSize (uint board_size) {
    return board_size == ::board_size;
  }

  void SetKomi (float komi) {
    float old_komi = base_board.Komi ();
    base_board.SetKomi (komi);
    if (komi != old_komi) {
      logger.LogLine("komi "+ToString(komi));
      logger.LogLine ("");
    }
  }

  void ClearBoard () {
    base_board.Clear ();
    Reset ();
    logger.NewLog ();
    logger.LogLine ("clear_board");
    logger.LogLine ("komi " + ToString (base_board.Komi()));
    logger.LogLine ("");
  }

  bool Play (Move move) {
    bool ok = base_board.IsReallyLegal (move);
    if (ok) {
      base_board.PlayLegal (move);
      base_board.Dump();
      logger.LogLine ("play " + move.ToGtpString());
      logger.LogLine ("");
    }
    return ok;
  }

  bool Undo () {
    bool ok = base_board.Undo ();
    if (ok) {
      logger.LogLine ("undo");
      logger.LogLine ("");
    }
    return ok;
  }

  const Board& GetBoard () const {
    return base_board;
  }

  Move Genmove (Player player) {
    logger.LogLine ("param.other seed " + ToString (random.GetSeed ()));
    
    base_board.SetActPlayer (player);
    Move m = Genmove ();

    if (m.IsValid ()) {
      CHECK (base_board.IsReallyLegal (m));
      base_board.PlayLegal (m);
      base_board.Dump();

      logger.LogLine ("reg_genmove " + player.ToGtpString() +
                      "   #? [" + m.GetVertex().ToGtpString() + "]");
      logger.LogLine ("play " + m.ToGtpString() + " # move " +
                      ToString(base_board.MoveCount()));
      logger.LogLine ("");
    }

    return m;
  }

  string BoardAsciiArt () {
    return base_board.ToAsciiArt();
  }
  

  Gtp::GoguiGfx LastPlayoutGfx (uint move_count) {
    vector<Move> last_playout = LastPlayout ();

    move_count = max(move_count, 0u);
    move_count = min(move_count, uint(last_playout.size()));

    Gtp::GoguiGfx gfx;

    rep(ii, move_count) {
      gfx.AddVariationMove (last_playout[ii].ToGtpString());
    }

    if (move_count > 0) {
      gfx.SetSymbol (last_playout[move_count-1].GetVertex().ToGtpString(),
                     Gtp::GoguiGfx::circle);
    }

//     rep (ii, mcmc.moves.size()) {
//       gfx.SetSymbol (mcmc.moves[ii].GetVertex().ToGtpString(),
//                      Gtp::GoguiGfx::triangle);
//     }


    return gfx;
  }

  void ShowGammas (Gtp::GoguiGfx& gfx) {
    Player pl = base_board.ActPlayer ();
    PrepareToPlayout ();
    NatMap <Vertex, double> p (0.0);
    ForEachNat (Vertex, v) {
      if (base_board.ColorAt (v) == Color::Empty()) {
        p [v] = sampler.act_gamma [v] [pl];
      }
    }
    p.ScalePositive ();
    ForEachNat (Vertex, v) {
      if (base_board.ColorAt (v) == Color::Empty()) {
        gfx.SetInfluence (v.ToGtpString(), p[v]);
      }
    }
  }

  double GetStatForVertex (Vertex /*v*/) {
    return (double)(rand()%201-100)/(double)100;
  }

  std::string GetStringForVertex (Vertex v) {
      return "Vertex: " + v.ToGtpString();
  }

private:
  friend class MctsGtp;

  // base board
  Board base_board;

  // logging
  Logger logger;
  

public:
  void Reset ();
  Move Genmove ();
  void DoNPlayouts (uint n);
  void PrepareToPlayout ();

  void DoOnePlayout ();
  Move ChooseMove ();

  void PlayMove (Move m);
  void Sync ();

  vector<Move> LastPlayout ();

  double Score (bool accurate);

  // TODO policy randomization
  Move ChooseLocalMove ();

private:
  
  TimeControl time_control;

  // playout
  Board play_board;
  vector<Move> playout_moves;

  Mcts mcts;

public:
  FastRandom random;
  Gammas gammas;
  Sampler sampler;

};

#endif /* ENGINE_H_ */
