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

class MctsEngine {
public:
  
  MctsEngine () : root (Player::white(), Vertex::any()) {
    resign_mean = -0.95;
    playout_count = 10000;
    reset_tree_on_genmove = false;
  }

  bool SetBoardSize (uint board_size) {
    return board_size == ::board_size;
  }

  void SetKomi (float komi) {
    float old_komi = full_board.komi ();
    full_board.set_komi (komi);
    if (komi != old_komi) {
      logger.LogLine("komi "+ToString(komi));
      logger.LogLine ("");
    }
  }

  void ClearBoard () {
    full_board.clear ();
    root.Reset ();
    logger.NewLog ();
    logger.LogLine ("clear_board");
    logger.LogLine ("komi " + ToString (full_board.komi()));
    logger.LogLine ("");
  }

  bool Play (Player pl, Vertex v) {
    bool ok = full_board.try_play (pl, v);
    if (ok) {
      logger.LogLine ("play " + pl.to_string() + " " + v.to_string());
      logger.LogLine ("");
    }
    return ok;
  }

  bool Undo () {
    bool ok = full_board.undo ();
    if (ok) {
      logger.LogLine ("undo");
      logger.LogLine ("");
    }
    return ok;
  }

  Vertex Genmove (Player player) {
    logger.LogLine ("global_random_seed");
    logger.LogLine ("#? [" + ToString (global_random.get_seed ()) + "]");
    if (reset_tree_on_genmove) root.Reset ();
    full_board.set_act_player(player); // TODO move player parameter to DoPlayouts
    DoNPlayouts (playout_count);

    //cerr << mcts.ToString (show_tree_min_updates, show_tree_max_children) << endl;

    Vertex v = BestMove (player);

    if (v != Vertex::resign ()) {
      bool ok = full_board.try_play (player, v);
      assert(ok);
    }
    logger.LogLine ("reg_genmove " + player.to_string());
    logger.LogLine ("#? [" + v.to_string() + "]");
    logger.LogLine ("play " + player.to_string() + " " + v.to_string());
    logger.LogLine ("");
    return v;
  }

  string BoardAsciiArt () {
    return full_board.to_string();
  }

  string TreeAsciiArt (float min_updates, float max_children) {
    MctsNode& act_root = FindRoot ();
    return act_root.RecToString (min_updates, max_children);
  }

  void DoNPlayouts (uint n) { // TODO first_player
    MctsNode& act_root = FindRoot ();
    rep (ii, n) {
      playout.DoOnePlayout (act_root, full_board.board());
    }
  }

  Gtp::GoguiGfx LastPlayoutGfx (uint move_count) {
    vector<Move> last_playout = playout.LastPlayout ();

    move_count = max(move_count, 0u);
    move_count = min(move_count, last_playout.size());

    Gtp::GoguiGfx gfx;

    rep(ii, move_count) {
      gfx.AddVariationMove (last_playout[ii].to_string());
    }

    if (move_count > 0) {
      gfx.SetSymbol (last_playout[move_count-1].get_vertex().to_string(),
                     Gtp::GoguiGfx::circle);
    }

    return gfx;
  }

private:

  Vertex BestMove (Player pl) {
    MctsNode& act_root = FindRoot ();
    const MctsNode& best_node = act_root.MostExploredChild (pl);

    return
      best_node.SubjectiveMean() < resign_mean ?
      Vertex::resign() :
      best_node.v;
  }

  MctsNode& FindRoot () {
    Board sync_board;
    MctsNode* act_root = &root;
    BOOST_FOREACH (Move m, full_board.MoveHistory ()) {
      act_root = act_root->AddFindChild (m, sync_board);
    }
    
    Player pl = full_board.act_player();
    act_root->EnsureAllPseudoLegalChildren (pl, full_board);
    act_root->RemoveIllegalChildren (pl, full_board);

    return *act_root;
  }

private:
  friend class MctsGtp;

  // parameters
  float print_update_count;
  float resign_mean;
  float playout_count;
  bool reset_tree_on_genmove;

  // base board
  FullBoard full_board;

  // logging
  Logger logger;
  
  // tree
  MctsNode root;
  
  // playout
  MctsPlayout playout;
};
