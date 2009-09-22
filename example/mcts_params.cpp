class MctsParams {
public:
  MctsParams (Gtp::ReplWithGogui& gtp) {
    uct_explore_coeff    = 1.0;
    mature_update_count  = 100.0;
    print_update_count   = 500.0;
    resign_mean          = -0.95;

    gtp.RegisterParam ("MCTS.params", "UCT_explore_coeff",
                       &uct_explore_coeff);
    gtp.RegisterParam ("MCTS.params", "Min_updates_to_have_children",
                       &mature_update_count);
    gtp.RegisterParam ("MCTS.params", "Min_updates_to_print",
                       &print_update_count);
    gtp.RegisterParam ("MCTS.params", "E(score)_to_resign",
                       &resign_mean);
  }

private:
  friend class Mcts;

  float uct_explore_coeff;
  float mature_update_count;
  float print_update_count;
  float resign_mean;
};
