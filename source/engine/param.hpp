#ifndef GO_PARAM_H
#define GO_PARAM_H

#include "utils.hpp"

class Param {
public:
  static float genmove_playouts;
  static bool  use_local;

  static bool  tree_use;
  static uint  tree_max_moves;
  static float tree_explore_coeff;
  static bool  tree_rave_update;
  static bool  tree_rave_use;
  static float tree_stat_bias;
  static float tree_rave_bias;
  static float tree_progressive_bias;
  static float tree_progressive_bias_prior;
  static float tree_rave_update_fraction;

  static float prior_update_count;
  static float prior_mean;

  static float mature_update_count;

  static float resign_mean;
};

#endif /* GO_PARAM_H */
