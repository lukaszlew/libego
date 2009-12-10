/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of Library of Effective GO routines - EGO library      *
 *                                                                           *
 *  Copyright 2006 and onwards, Lukasz Lew                                   *
 *                                                                           *
 *  EGO library is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation; either version 2 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  EGO library is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with EGO library; if not, write to the Free Software               *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor,                           *
 *  Boston, MA  02110-1301  USA                                              *
 *                                                                           *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
namespace Mcts {
  class Judge;
}

class MctsEngine {
public:
  
  MctsEngine ()
    : root (Player::White(), Vertex::Invalid()),
      random (TimeSeed()),
      playout(random),
      time_left (0.0),
      time_stones (-1),
      playouts_per_second (50000)
  {
  }

  bool SetBoardSize (uint board_size) {
    return board_size == ::board_size;
  }

  void SetKomi (float komi) {
    float old_komi = full_board.GetBoard().Komi ();
    full_board.SetKomi (komi);
    if (komi != old_komi) {
      logger.LogLine("komi "+ToString(komi));
      logger.LogLine ("");
    }
  }

  void ClearBoard () {
    full_board.Clear ();
    root.Reset ();
    logger.NewLog ();
    logger.LogLine ("clear_board");
    logger.LogLine ("komi " + ToString (full_board.GetBoard().Komi()));
    logger.LogLine ("");
  }

  bool Play (Move move) {
    bool ok = full_board.Play (move);
    if (ok) {
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

  Vertex Genmove (Player player) {
    logger.LogLine ("random_seed     #? [" + ToString (random.GetSeed ()) + "]");
    if (Param::reset_tree_on_genmove) root.Reset ();

    int playouts = Param::genmove_playouts;
    if (time_stones [player] == 0 && time_left [player] < 60.0) {
      playouts = min (playouts,
                      int (time_left [player] / 30.0 * playouts_per_second));
      playouts = max (playouts, 1000);
    }
    //cerr << "Playouts: " << playouts << endl;
    DoNPlayouts (playouts, player);

    //cerr << mcts.ToString (show_tree_min_updates, show_tree_max_children) << endl;

    Vertex v = BestMove (player);

    if (v != Vertex::Invalid ()) {
      CHECK (full_board.Play (Move (player, v)));
    }
    logger.LogLine ("reg_genmove " + player.ToGtpString() +
                    "   #? [" + v.ToGtpString() + "]");
    logger.LogLine ("play " + Move (player, v).ToGtpString());
    logger.LogLine ("");
    return v;
  }

  string BoardAsciiArt () {
    return full_board.GetBoard().ToAsciiArt();
  }

  string TreeAsciiArt (float min_updates, float max_children) {
    MctsNode& act_root = FindRoot ();
    return act_root.RecToString (min_updates, max_children);
  }

  void DoNPlayouts (uint n) {
    DoNPlayouts (n, full_board.GetBoard().ActPlayer());
  }

  void DoNPlayouts (uint n, Player first_player) {
    MctsNode& act_root = FindRoot ();
    rep (ii, n) {
      playout.DoOnePlayout (act_root, full_board.GetBoard(), first_player);
    }
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

    return gfx;
  }

  void TimeLeft (Player pl, float seconds, int stones) {
    time_left [pl] = seconds;
    time_stones [pl] = stones;
  }

private:

  Vertex BestMove (Player pl) {
    MctsNode& act_root = FindRoot ();
    const MctsNode& best_node = act_root.MostExploredChild (pl);

    return
      best_node.SubjectiveMean() < Param::resign_mean ?
      Vertex::Invalid() :
      best_node.v;
  }

  MctsNode& FindRoot () {
    Board sync_board;
    MctsNode* act_root = &root;
    BOOST_FOREACH (Move m, full_board.MoveHistory ()) {
      act_root = act_root->AddFindChild (m, sync_board);
    }
    
    Player pl = full_board.GetBoard().ActPlayer();
    act_root->EnsureAllPseudoLegalChildren (pl, full_board.GetBoard());
    act_root->RemoveIllegalChildren (pl, full_board);

    return *act_root;
  }


private:
  friend class MctsGtp;
  friend class Mcts::Judge;

  // base board
  FullBoard full_board;

  // logging
  Logger logger;
  
  // tree
  MctsNode root;
  
  // playout
  FastRandom random;
  MctsPlayout playout;

  NatMap <Player, float> time_left;
  NatMap <Player, int>   time_stones;
  float playouts_per_second;
};
