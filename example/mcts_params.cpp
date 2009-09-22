class MctsParams {
public:
  MctsParams (Gtp::ReplWithGogui& gtp) {
    explore_rate                   = 1.0;
    mature_update_count_threshold  = 100.0;
    min_visit                      = 2500;
    resign_mean                    = -0.95;

    gtp.RegisterParam ("MCTS.params", "explore_rate",
                       &explore_rate);
    gtp.RegisterParam ("MCTS.params", "#_updates_to_promote",
                       &mature_update_count_threshold);
    gtp.RegisterParam ("MCTS.params", "print_min_visit",
                       &min_visit);
  }

private:
  friend class Mcts;

  float explore_rate;
  float mature_update_count_threshold;
  float resign_mean;
  float min_visit;
};
