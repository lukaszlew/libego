#pragma once

#include "board.cpp"


// playout_t_

#ifdef NDEBUG
static const bool playout_ac = false;
#endif


#ifdef DEBUG
static const bool playout_ac = true;
#endif

class playout_t { // board with incremental fast random move generator

public:

  board_t*    board;

  v::t        empty_v[board_area];
  uint        empty_v_cnt;

  v::t        rejected_v[board_area];
  uint        rejected_v_cnt;

  void check () {
    if (!playout_ac) return;

    bool noticed[v::cnt];

    assert    (empty_v_cnt <= board_area);
    assert (rejected_v_cnt <= board_area);

    rep (v, v::cnt) noticed[v] = false;

    rep (k, empty_v_cnt) {
      assert (noticed[empty_v[k]] == false);
      noticed[empty_v[k]] = true;
    }

    rep (k, rejected_v_cnt) {
      assert (noticed[rejected_v[k]] == false);
      noticed[rejected_v[k]] = true;
    }

    v_for_each (v) // TODO on_board
      assert ((board->color_at[v] == color::empty) == noticed[v]);
  }

  playout_t (board_t* board) { // copying constructor makes no improvement as randomization is a key
    this->board = board;

    init ();
  }

  void init () {
    empty_v_cnt    = 0;
    rejected_v_cnt = 0;

    v_for_each (v)              // TODO ...on_board
      if (board->color_at[v] == color::empty) 
        add_empty_v (v);
  }

  void add_empty_v (v::t v) {
    uint ii;

    empty_v_cnt++;
    ii = pm::rand () % empty_v_cnt;         // TODO improve speed here
    empty_v[empty_v_cnt-1] = empty_v[ii];
    empty_v[ii] = v;
    assertc (playout_ac, empty_v_cnt <= board_area);
  }

  void run (player::t first_pl) {
    v::t  v;
    bool  was_pass[player::cnt];

    player_for_each (pl) was_pass [pl] = 0;



    #define pp(player) {                    \
      v = play_one (player);                \
      cout << endl;                         \
      board->print(v);                      \
      getchar();                            \
      was_pass[player] = (v == v::pass);    \
    } 
    
    if (first_pl == player::white) pp (player::white);

    rep (move_no, board_area) { // to avoid rare loopy playouts
      assertc (playout_ac, board->captured_cnt == 0);

      player_for_each (pl) pp (pl);

      if (was_pass [player::black] & was_pass [player::white]) break;  // TODO condition here
    }
  }

  v::t play_one (player::t player) {
    v::t                   v;

    // TODO  too many if's inside
    while (empty_v_cnt > 0) {   // TODO neutralize this "if"
      v = empty_v [--empty_v_cnt];

      // TODO try two phases 1. any eye is bad, play_no_eye 2. play eyes      
      if (board->is_eyelike (player, v) || 
          board->play (player, v) == board_t::play_ss_suicide) 
      // then
      {
        qq ("REJECT\n");
        board->print (v);
        getchar ();

        rejected_v[rejected_v_cnt++] = v;
      } else {
        qq ("OK");
        while (board->captured_cnt > 0) // TODO integrate captured into "play"
          add_empty_v (board->captured[--board->captured_cnt]);

        return v;
      }
    }

    memcpy (empty_v, rejected_v, sizeof(v::cnt) * rejected_v_cnt);
    empty_v_cnt = rejected_v_cnt;
    rejected_v_cnt = 0;

    cout << "MEMCPY " << empty_v_cnt << endl;

    while (empty_v_cnt > 0) {   // TODO neutralize this "if"
      v = empty_v [--empty_v_cnt];

      // TODO try two phases 1. any eye is bad, play_no_eye 2. play eyes      
      if (board->is_eyelike (player, v) || 
          board->play (player, v) == board_t::play_ss_suicide) 
      // then
      {
        qq ("REJECT 2\n");
        board->print (v);
        getchar ();

        rejected_v[rejected_v_cnt++] = v;
      } else {
        qq ("OK 2");
        while (board->captured_cnt > 0) // TODO integrate captured into "play"
          add_empty_v (board->captured[--board->captured_cnt]);

        return v;
      }
    }

    board->check_no_more_legal (player); // powerfull check

    return v::pass;
  }



};


#ifdef PLAYOUT_TEST

int main () { 
  board_t board[2];
  uint win_cnt [player::cnt];
  
  player_for_each (pl) win_cnt [pl] = 0;

  (board+1)->clear ();
  (board+1)->set_komi (-2);
  
  rep (ii, 400000) {
    board->load (board+1);

    playout_t playout (board);
    playout.run (player::black);

    win_cnt [board->winner ()] ++; // TODO this is a way too costly // (we have empty tab, there is a better way)
  }

  cout << "black wins = " << win_cnt [player::black] << endl
       << "white wins = " << win_cnt [player::white] << endl;

  return 0;
}

#endif


/*

  Mam bardzo rozgaleziony algorytm, dlaczego?
1. limit na dlugosc playoutu
2. 2 pass
3. szukamy ruchu while empty_v_cnt > 0
4. eyelike state
5. play result
6. while captured

Potrzebujemy:
  Dobrego kryterium konca gry.

Idealny algorytm:
 1. good_move_cnt > 0 ?  
    
*/
