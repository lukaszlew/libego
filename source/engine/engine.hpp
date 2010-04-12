//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef ENGINE_H_
#define ENGINE_H_

#include "to_string.hpp"
#include "gtp_gogui.hpp"
#include "ego.hpp"
#include "time_control.hpp"
#include "mcts_tree.hpp"

class Engine {
public:
  Engine ();

  bool Reset (uint board_size);
  void SetKomi (float komi);
  bool Play (Move move);
  Move Genmove (Player player);
  bool Undo ();

  void DoPlayoutMove ();

  const Board& GetBoard () const;

  // Playout functions
  Move ChooseBestMove ();
  void DoNPlayouts (uint n);
  void SyncRoot ();
  void PrepareToPlayout ();
  void DoOnePlayout (bool use_tree, bool update_tree);
  Move ChooseMctsMove (bool* tree_phase);
  void PlayMove (Move m);
  double Score (bool tree_phase);

  enum InfluenceType {
    NoInfluence,
    MctsN,
    MctsMean,
    RaveN,
    RaveMean,
    Bias,
    MctsPolicyMix,
    SamplerMoveProb,
    PatternGammas,
    CompleteGammas,
    PlayoutTerritory,
    MctsTerritory
  };

  void GetInfluence (InfluenceType type, NatMap <Vertex,double>& influence);

  std::string GetStringForVertex (Vertex v);
  vector<Move> LastPlayout ();

  void EnsureAllLegalChildren (MctsNode* node, const Board& board, const Sampler& sampler);
  void RemoveIllegalChildren (MctsNode* node, const Board& board);

private:
  void EstimateTerritory (NatMap<Vertex, double>& influence, bool use_tree);

  TimeControl time_control;
  Gammas gammas;

  FastRandom random;

  MctsNode root;
  Sampler sampler;

  Board base_board;
  MctsNode* base_node;
  
  Board playout_board;
  MctsNode* playout_node;

  vector<Move> playout_moves;
  MctsTrace trace;

  friend class MctsGtp;
};

#endif /* ENGINE_H_ */
