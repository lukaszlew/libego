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

const bool mcts_ac = true;

// -----------------------------------------------------------------------------

class Mcts {
public:
  
  Mcts (FullBoard& full_board_)
    : full_board (full_board_),
      root (Player::white(), Vertex::any()),
      act_root (NULL)
  {
    uct_explore_coeff    = 1.0;
    mature_update_count  = 100.0;
    print_update_count   = 500.0;
    resign_mean          = -0.95;
  }

  ~Mcts () {
    if (act_root) delete act_root; // TODO scoped_ptr
  }

  void DoNPlayouts (uint n) { // TODO first_player
    Reset ();
    rep (ii, n) {
      DoOnePlayout ();
    }
  }

  string ToString () {
    return act_root->RecToString (print_update_count);
  }

  Vertex BestMove (Player pl) {
    // TODO sync here wit full_Board as well
    // Find best move from the act_root and print tree.
    const MctsNode& best_node = act_root->MostExploredChild (pl);

    return
      (pl.subjective_score (best_node.stat.mean()) < resign_mean) ? 
      Vertex::resign () :
      best_node.v;
  }

  vector<Move> NewPlayout () {
    // TODO replace it with MCTS playout.
    LightPlayout::MoveHistory history;

    Board playout_board;
    playout_board.load (&full_board.board());
    LightPlayout playout (&playout_board);
    playout.Run (history);
    
    return history.AsVector ();
  }

private:

//   void Synchronize () {
//     act_root = &root;
//     act_root_path = full_board.MoveHistory ();
//     FOREACH (Move m, act_root_path) {
//       MctsNode* new_node = act_root->FindChild (jest ruch m);
//       if (new_node) {
//         act_root = new_node;
//       } else {
//         act_root = act_root->AddChild (m->get_player(), m->get_vertex());
//       }
//     }

//     // rekursywne upgrade do has_all_legal_children
//   }

  void Reset () {
    Player act_player = full_board.board().act_player();
    // prepare act_root of the tree
    if (act_root) delete act_root;
    act_root = new MctsNode(act_player.other(), Vertex::any());
    act_root_path = full_board.MoveHistory ();

    act_root->AddAllPseudoLegalChildren (act_player, full_board.board());
    act_root->RemoveIllegalChildren (act_player, full_board);
  }

  void DoOnePlayout (){
    // Prepare simulation board and tree iterator.
    play_board.load (&full_board.board());
    trace.clear();
    trace.push_back(act_root);

    // descent the MCTS tree
    while(ActNode().has_all_legal_children [play_board.act_player()]) {
      if (!DoTreeMove ()) return;
    }

    if (play_board.both_player_pass()) {
      UpdateTree (play_board.tt_winner().to_score());
      return;
    }
    
    // Is leaf is ready to expand ?
    if (ActNode().stat.update_count() > mature_update_count) {
      Player pl = play_board.act_player();
      assertc (mcts_ac, pl == ActNode().player.other());

      ActNode().AddAllPseudoLegalChildren (pl, play_board);

      if (!DoTreeMove ()) return; // Descend one more level.
    }
    // TODO check for pass x 2 here as well

    // Finish with regular playout.
    LightPlayout (&play_board).Run();
    
    // Update score.
    UpdateTree (play_board.playout_winner().to_score());
  }
  
  bool DoTreeMove () {
    Player pl = play_board.act_player ();
    MctsNode& uct_child = ActNode().FindUctChild (pl, uct_explore_coeff);

    assertc (mcts_ac, play_board.is_pseudo_legal (pl, uct_child.v));

    // Try to play it on the board
    play_board.play_legal (pl, uct_child.v);
    if (play_board.last_move_status != Board::play_ok) { // large suicide
      assertc (mcts_ac, play_board.last_move_status == Board::play_suicide);
      assertc (mcts_ac, !uct_child.HaveChildren ());
      assertc (mcts_ac, uct_child.stat.update_count() == Stat::prior_update_count);
      // Remove in case of large suicide.
      ActNode().RemoveChild (&uct_child);
      return false;
    }

    // Update tree itreatror.
    trace.push_back (&uct_child);
    return true;
  }

  void UpdateTree (float score) { // score: black -> 1, white -> -1
    FOREACH (MctsNode* node, trace) {
      node->stat.update (score);
    }
  }

  MctsNode& ActNode() {
    assertc (mcts_ac, trace.size() > 0);
    return *trace.back ();
  }

private:
  friend class MctsGtp;

  // parameters
  float uct_explore_coeff;
  float mature_update_count;
  float print_update_count;
  float resign_mean;

  // base board
  FullBoard& full_board;
  
  // playout
  Board play_board;

  // tree
  MctsNode root;
  MctsNode* act_root;
  vector <Move> act_root_path;
  vector <MctsNode*> trace;
};
