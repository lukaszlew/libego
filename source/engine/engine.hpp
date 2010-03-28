#ifndef ENGINE_H_
#define ENGINE_H_
//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "mcts_playout.hpp"
#include "logger.hpp"
#include "to_string.hpp"

class Engine {
public:
  
  Engine () : playout (full_board) { }

  bool SetBoardSize (uint board_size) {
    return board_size == ::board_size;
  }

  void SetKomi (float komi) {
    float old_komi = full_board.Komi ();
    full_board.SetKomi (komi);
    if (komi != old_komi) {
      logger.LogLine("komi "+ToString(komi));
      logger.LogLine ("");
    }
  }

  void ClearBoard () {
    full_board.Clear ();
    playout.Reset ();
    logger.NewLog ();
    logger.LogLine ("clear_board");
    logger.LogLine ("komi " + ToString (full_board.Komi()));
    logger.LogLine ("");
  }

  bool Play (Move move) {
    bool ok = full_board.IsReallyLegal (move);
    if (ok) {
      full_board.PlayLegal (move);
      full_board.Dump();
      logger.LogLine ("play " + move.ToGtpString());
      logger.LogLine ("");
    }
    return ok;
  }

  bool Undo () {
    bool ok = full_board.Undo ();
    if (ok) {
      logger.LogLine ("undo");
      logger.LogLine ("");
    }
    return ok;
  }

  const Board& GetBoard () const {
    return full_board;
  }

  Move Genmove (Player player) {
    logger.LogLine ("param.other seed " + ToString (playout.random.GetSeed ()));
    
    full_board.SetActPlayer (player);
    Move m = playout.Genmove ();

    if (m.IsValid ()) {
      CHECK (full_board.IsReallyLegal (m));
      full_board.PlayLegal (m);
      full_board.Dump();

      logger.LogLine ("reg_genmove " + player.ToGtpString() +
                      "   #? [" + m.GetVertex().ToGtpString() + "]");
      logger.LogLine ("play " + m.ToGtpString() + " # move " +
                      ToString(full_board.MoveCount()));
      logger.LogLine ("");
    }

    return m;
  }

  string BoardAsciiArt () {
    return full_board.ToAsciiArt();
  }
  

  Gtp::GoguiGfx LastPlayoutGfx (uint move_count) {
    vector<Move> last_playout = playout.LastPlayout ();

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

//     rep (ii, playout.mcmc.moves.size()) {
//       gfx.SetSymbol (playout.mcmc.moves[ii].GetVertex().ToGtpString(),
//                      Gtp::GoguiGfx::triangle);
//     }


    return gfx;
  }

  void ShowGammas (Gtp::GoguiGfx& gfx) {
    Player pl = full_board.ActPlayer ();
    playout.PrepareToPlayout ();
    NatMap <Vertex, double> p (0.0);
    ForEachNat (Vertex, v) {
      if (full_board.ColorAt (v) == Color::Empty()) {
        p [v] = playout.sampler.act_gamma [v] [pl];
      }
    }
    p.ScalePositive ();
    ForEachNat (Vertex, v) {
      if (full_board.ColorAt (v) == Color::Empty()) {
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
  Board full_board;

  // logging
  Logger logger;
  
  // playout
  MctsPlayout playout;

};

#endif /* ENGINE_H_ */
