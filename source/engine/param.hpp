namespace Param {
  bool mcmc_use    = true;
  bool mcmc_update = true;
  float mcmc_update_fraction = 0.5;
  float mcmc_explore_coeff = 10.0;
  float mcmc_prob_8_nbr = 0.8;

  // tree
  bool use_mcts_tree = true;

  float uct_explore_coeff = 0.0;

  float mcts_bias = 0.0;
  float rave_bias = 0.001;
  bool  update_rave = true;
  bool  use_rave  = true;  
  bool  use_local  = true;  

  float prior_update_count = 10.0;
  float prior_mean = 1.0;

  float mature_update_count = 10.0;

  float resign_mean = -0.90;
  float genmove_playouts = 100000;
  bool reset_tree_on_genmove = true; // TODO memory problems 
}
