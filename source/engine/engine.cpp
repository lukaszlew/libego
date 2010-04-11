//
// Copyright 2006 and onwards, Lukasz Lew
//

#include <boost/foreach.hpp>

#include "engine.hpp"

Engine::Engine () :
  random (TimeSeed()),
  root (Player::White(), Vertex::Any (), 0.0),
  sampler (playout_board, gammas)
{
  CHECK (Reset (board_size));
}


bool Engine::Reset (uint board_size) {
  base_board.Clear ();
  root.Reset ();
  base_node = &root; // easy SyncRoot
  return board_size == ::board_size;
}


void Engine::SetKomi (float komi) {
  base_board.SetKomi (komi);
}


bool Engine::Play (Move move) {
  CHECK (move.IsValid ());
  bool ok = base_board.IsReallyLegal (move);
  if (ok) {
    base_board.PlayLegal (move);
    SyncRoot ();
    base_board.Dump();
  }
  return ok;
}


Move Engine::Genmove (Player player) {
  base_board.SetActPlayer (player);
  Move move = ChooseBestMove ();
  if (move.IsValid ()) {
    CHECK (Play (move));
  }
  return move;
}


bool Engine::Undo () {
  bool ok = base_board.Undo ();
  if (ok) {
    SyncRoot ();
  }
  return ok;
}


void Engine::DoPlayoutMove () {
  PrepareToPlayout ();
  FastRandom fr;
  Vertex v = sampler.SampleMove (fr);
  Move move = Move (base_board.ActPlayer (), v);
  CHECK (move.IsValid ());
  CHECK (Play (move));
}


const Board& Engine::GetBoard () const {
  return base_board;
}


void Engine::GetInfluence (InfluenceType type, 
                           NatMap <Vertex,double>& influence)
{
  if (type == SamplerMoveProb) {
    PrepareToPlayout ();
    sampler.SampleMany (10000, influence);
    return;
  }

  if (type == PatternGammas) {
    PrepareToPlayout ();
    sampler.GetPatternGammas (influence, false);
    return;
  }

  if (type == CompleteGammas) {
    PrepareToPlayout ();
    sampler.GetPatternGammas (influence, true);
    return;
  }

  float log_val = log (base_node->stat.update_count ());

  ForEachNat (Vertex, v) {
    Move m = Move (base_board.ActPlayer (), v);
    if (base_board.IsLegal (m)) {
      MctsNode* node = base_node->FindChild (m);
      if (node != NULL) {
        switch (type) {
        case NoInfluence:
          influence [v] = nan ("");
          break;
        case MctsN:
          influence [v] = log10 (node->stat.update_count());
          break;
        case MctsMean:
          influence [v] = node->stat.mean();
          break;
        case RaveN:
          influence [v] = log10 (node->rave_stat.update_count());
          break;
        case RaveMean:
          influence [v] = node->rave_stat.mean();
          break;
        case Bias:
          influence [v] = node->bias;
          break;
        case MctsPolicyMix:
          influence [v] = node->SubjectiveRaveValue (base_board.ActPlayer(), log_val);
          break;
        case SamplerMoveProb:
        case PatternGammas:
        case CompleteGammas:
          CHECK (false);
        }
      } else {
        influence [v] = nan ("");
      }
    } else {
      influence [v] = nan ("");
    }
  }
}


std::string Engine::GetStringForVertex (Vertex v) {
  Move m = Move (base_board.ActPlayer (), v);
  MctsNode* node = base_node->FindChild (m);
  if (node != NULL) {
    return node->GuiString ();
  } else {
    return "";
  }
}


Move Engine::ChooseBestMove () {
  // TODO Garbage collection of old tree here !
  Player player = base_board.ActPlayer ();
  int playouts = time_control.PlayoutCount (player);
  DoNPlayouts (playouts);

  const MctsNode& best_node = base_node->MostExploredChild (player);

  return
    best_node.SubjectiveMean() < Param::resign_mean ?
    Move::Invalid() :
    Move (player, best_node.v);
}


void Engine::DoNPlayouts (uint n) {
  rep (ii, n) {
    DoOnePlayout ();
  }
}


void Engine::SyncRoot () {
  // TODO replace this by FatBoard
  Board sync_board;
  Sampler sampler(sync_board, gammas);
  sampler.NewPlayout ();

  base_node = &root;
  BOOST_FOREACH (Move m, base_board.Moves ()) {
    sync_board.SetActPlayer (m.GetPlayer());
    EnsureAllLegalChildren (base_node, sync_board, sampler);
    base_node = base_node->FindChild (m);
    CHECK (sync_board.IsLegal (m));
    sync_board.PlayLegal (m);
    sampler.MovePlayed();
  }

  EnsureAllLegalChildren (base_node, base_board, sampler);
  RemoveIllegalChildren (base_node, base_board);
  cerr << endl << base_node->RecToString (100, 6) << endl;
}


void Engine::DoOnePlayout () {
  PrepareToPlayout();

  // do the playout
  while (true) {
    if (playout_board.BothPlayerPass()) break;
    if (playout_board.MoveCount() >= 3*Board::kArea) return;

    Move m = Move::Invalid ();
    if (!m.IsValid()) m = ChooseMctsMove ();
    if (!m.IsValid()) m = Move (playout_board.ActPlayer (), sampler.SampleMove (random));
    PlayMove (m);
  }

  double score = Score ();
  trace.UpdateTraceRegular (score);
}


void Engine::PrepareToPlayout () {
  playout_board.Load (base_board);
  playout_moves.clear();
  sampler.NewPlayout ();

  trace.Reset (*base_node);
  playout_node = base_node;
  tree_phase = Param::tree_use;
}

Move Engine::ChooseMctsMove () {
  Player pl = playout_board.ActPlayer();

  if (!tree_phase) {
    return Move::Invalid();
  }

  if (!playout_node->has_all_legal_children [pl]) {
    if (!playout_node->ReadyToExpand ()) {
      tree_phase = false;
      return Move::Invalid();
    }
    ASSERT (pl == playout_node->player.Other());
    EnsureAllLegalChildren (playout_node, playout_board, sampler);
  }

  MctsNode& uct_child = playout_node->BestRaveChild (pl);
  trace.NewNode (uct_child);
  playout_node = &uct_child;
  ASSERT (uct_child.v != Vertex::Any());
  return Move (pl, uct_child.v);
}

void Engine::EnsureAllLegalChildren (MctsNode* node, const Board& board, const Sampler& sampler) {
  Player pl = board.ActPlayer ();
  if (node->has_all_legal_children [pl]) return;
  empty_v_for_each_and_pass (&board, v, {
      // superko nodes have to be removed from the tree later
      if (board.IsLegal (pl, v)) {
      double bias = sampler.Probability (pl, v);
      node->AddChild (MctsNode(pl, v, bias));
      }
      });
  node->has_all_legal_children [pl] = true;
}


void Engine::RemoveIllegalChildren (MctsNode* node, const Board& board) {
  Player pl = board.ActPlayer ();
  ASSERT (node->has_all_legal_children [pl]);

  MctsNode::ChildrenList::iterator child = node->children.begin();
  while (child != node->children.end()) {
    if (child->player == pl && !board.IsReallyLegal (Move (pl, child->v))) {
      node->children.erase (child++);
    } else {
      ++child;
    }
  }
}


void Engine::PlayMove (Move m) {
  ASSERT (playout_board.IsLegal (m));
  playout_board.PlayLegal (m);

  trace.NewMove (m);
  sampler.MovePlayed ();

  playout_moves.push_back (m);
}


vector<Move> Engine::LastPlayout () {
  return playout_moves;
}


double Engine::Score () {
  // TODO game replay i update wszystkich modeli
  double score;
  if (tree_phase) {
    score = playout_board.TrompTaylorWinner().ToScore();
  } else {
    int sc = playout_board.PlayoutScore();
    score = Player::WinnerOfBoardScore (sc).ToScore (); // +- 1
    score += double(sc) / 10000.0; // small bonus for bigger win.
  }
  return score;
}

