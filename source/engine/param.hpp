#ifndef GO_PARAM_H
#define GO_PARAM_H

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

  static bool  mcmc_update;
  static float mcmc_update_fraction;
  static bool  mcmc_use;
  static uint  mcmc_max_moves;
  static float mcmc_stat_bias;
  static float mcmc_rave_bias;
  static float mcmc_explore_coeff;

  static float prior_update_count;
  static float prior_mean;

  static float mature_update_count;

  static float resign_mean;
  static bool reset_tree_on_genmove;
};

#endif /* GO_PARAM_H */
