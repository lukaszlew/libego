#ifndef _PLAYOUT_H_
#define _PLAYOUT_H_

enum playout_status_t { pass_pass, mercy, too_long };

template <typename Policy> class Playout {
public:
  Policy*  policy;
  Board*   board;
  Move*    move_history;
  uint     move_history_length;

  Playout (Policy* policy_, Board*  board_) : policy (policy_), board (board_) {}

  all_inline
  void play_move () {
    policy->prepare_vertex ();
    
    while (true) {
      Player   act_player = board->act_player ();
      Vertex   v          = policy->next_vertex ();

      if (board->is_pseudo_legal (act_player, v) == false) {
        policy->bad_vertex (v);
        continue;
      } else {
        board->play_legal (act_player, v);
        policy->played_vertex (v);
        break;
      }
    }
  }

  all_inline
  playout_status_t run () {
    uint begin_move_no = board->move_no;
    move_history = board->move_history + board->move_no;
    
    policy->begin_playout (board);
    while (true) {
      play_move ();
      
      if (board->both_player_pass ()) {
        move_history_length = board->move_no - begin_move_no;
        policy->end_playout (pass_pass);
        return pass_pass;
      }
      
      if (board->move_no >= max_playout_length) {
        move_history_length = board->move_no - begin_move_no;
        policy->end_playout (too_long);
        return too_long;
      }
      
      if (use_mercy_rule && uint (abs (board->approx_score ())) > mercy_threshold) {
        move_history_length = board->move_no - begin_move_no;
        policy->end_playout (mercy);
        return mercy;
      }
    }
  }
  
};

// ----------------------------------------------------------------------

class SimplePolicy {
protected:

  static FastRandom fr; // TODO make it non-static

  uint to_check_cnt;
  uint act_evi;

  Board* board;
  Player act_player;

public:

  SimplePolicy ();

  void begin_playout (Board* board_);
  void prepare_vertex ();
  Vertex next_vertex ();
  void bad_vertex (Vertex);
  void played_vertex (Vertex);
  void end_playout (playout_status_t);
};

typedef Playout<SimplePolicy> SimplePlayout;

#endif
