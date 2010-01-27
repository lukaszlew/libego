namespace Param {
  float genmove_playouts = 10000;
  bool  use_local  = true;  

  bool  tree_use = true;
  uint  tree_max_moves   = 200;
  float tree_explore_coeff = 0.0;
  bool  tree_rave_update = true;
  bool  tree_rave_use    = true;  
  float tree_stat_bias   = 0.0;
  float tree_rave_bias   = 0.001;

  bool  mcmc_update = false;
  float mcmc_update_fraction = 0.5;
  bool  mcmc_use    = false;
  uint  mcmc_max_moves = 4;
  float mcmc_stat_bias = 0.0;
  float mcmc_rave_bias = 0.01;
  float mcmc_explore_coeff = 0.0;



  float prior_update_count = 10.0;
  float prior_mean = 1.0;

  float mature_update_count = 10.0;

  float resign_mean = -0.90;
  bool reset_tree_on_genmove = true; // TODO memory problems 
}
