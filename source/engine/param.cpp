#include "param.hpp"

  float Param::genmove_playouts = 10000;
  bool  Param::use_local  = true;  

  bool  Param::tree_use = true;
  uint  Param::tree_max_moves   = 200;
  float Param::tree_explore_coeff = 0.0;
  bool  Param::tree_rave_update = true;
  bool  Param::tree_rave_use    = true;  
  float Param::tree_stat_bias   = 0.0;
  float Param::tree_rave_bias   = 0.001;
  float Param::tree_progressive_bias = 500.0;

  bool  Param::mcmc_update = false;
  float Param::mcmc_update_fraction = 0.5;
  bool  Param::mcmc_use    = false;
  uint  Param::mcmc_max_moves = 4;
  float Param::mcmc_stat_bias = 0.0;
  float Param::mcmc_rave_bias = 0.01;
  float Param::mcmc_explore_coeff = 0.0;

  float Param::prior_update_count = 10.0;
  float Param::prior_mean = 1.0;

  float Param::mature_update_count = 10.0;

  float Param::resign_mean = -0.90;
  bool  Param::reset_tree_on_genmove = true; // TODO memory problems 

