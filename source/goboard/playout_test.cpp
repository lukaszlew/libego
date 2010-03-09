#include "playout_test.hpp"

void PlayoutTest (bool print_moves) {
  Board empty;
  Board board;
  FastRandom random (123);
  NatMap <Player, uint> win_cnt (0);
  uint move_count = 0;
  uint move_count2 = 0;
  uint hash_changed_count = 0;
  Gammas gammas;
  Sampler sampler (board, gammas);
  uint n = 10000;
  if (board_size == 19) {
    n = 1000;
  }

  rep (ii, n) {
    board.Load (empty);
    sampler.NewPlayout ();

    // Plaout loop
    while (!board.BothPlayerPass ()) {
      move_count2 += 1;
      FastStack<Vertex, Board::kArea> legals; // TODO pass
      Player pl = board.ActPlayer();

      // legal moves
      rep (jj, board.EmptyVertexCount()) {
        Vertex v = board.EmptyVertex (jj);
        CHECK2 (board.KoVertex() == v ||
                board.IsLegal (pl, v) == board.Hash3x3At(v).IsLegal(pl), {
                  board.DebugPrint (v);
                });
        if (v != Vertex::Pass () &&
            board.IsLegal (pl, v) &&
            !board.IsEyelike (pl, v)) {
          legals.Push(v);
        }
      }

      // random move
      Vertex sampler_v = sampler.SampleMove(random);
      CHECK (board.IsLegal (pl, sampler_v));


      CHECK2 (fabs (sampler.act_gamma_sum [pl] - legals.Size()) < 0.000001, {
        board.DebugPrint (board.LastVertex());
        WW (sampler.act_gamma_sum [pl]);
        WW (legals.Size());
        WW (move_count2);
      });

      Vertex v;
      uint random_idx = 999;
      if (legals.Size() == 0) {
        v = Vertex::Pass ();
      } else {
        random_idx = random.GetNextUint (legals.Size());
        v = legals.Remove (random_idx);
      }

      // play_it
      board.PlayLegal (pl, v);
      sampler.MovePlayed ();

      hash_changed_count += board.Hash3x3ChangedCount ();

      if (print_moves && ii < 1000) {
        cerr << move_count2 << ":"
             << v.ToGtpString () << " "
             << "(" << random_idx << "/" << legals.Size () + 1 << ") "
             << board.Hash3x3ChangedCount ()
             << endl;
      }
    }

    win_cnt [board.PlayoutWinner ()] ++;
    move_count += board.MoveCount();
  }

  // Report and regression checks.
  cerr
    << "board_test ok: "
    << win_cnt [Player::Black ()] << "/"
    << win_cnt [Player::White ()] << " "
    << move_count << " "
    << hash_changed_count << " "
    << endl;

  if (board_size == 9) {
    CHECK (win_cnt [Player::Black()] == 4436);
    CHECK (win_cnt [Player::White()] == 5564);
    CHECK (move_count  == move_count2);
    CHECK (move_count2 == 1109454);
    CHECK (hash_changed_count == 3702383);
  } else if (board_size == 19) {
    CHECK (win_cnt [Player::Black()] == 476);
    CHECK (win_cnt [Player::White()] == 524);
    CHECK (move_count  == move_count2);
    CHECK (move_count2 == 454567);
    CHECK (hash_changed_count == 1689259);
  } else {
    CHECK (false);
  }
}



void SamplerPlayoutTest (bool print_moves) {
  Board empty;
  Board board;
  FastRandom random (123);
  NatMap <Player, uint> win_cnt (0);
  uint move_count = 0;
  uint move_count2 = 0;
  uint hash_changed_count = 0;
  Gammas gammas;
  Sampler sampler (board, gammas);

  uint n = 10000;
  if (board_size == 19) n = 1000;

  rep (ii, n) {
    board.Load (empty);
    sampler.NewPlayout();

    // Plaout loop
    while (!board.BothPlayerPass ()) {
      move_count2 += 1;
      Player pl = board.ActPlayer();


      // random move
      Vertex v = sampler.SampleMove(random);
      CHECK (board.IsLegal (pl, v));

      // play_it
      board.PlayLegal (pl, v);
      sampler.MovePlayed();

      hash_changed_count += board.Hash3x3ChangedCount ();

      if (print_moves && ii < 1000) {
        cerr << move_count2 << ":"
             << v.ToGtpString () << " "
             << board.Hash3x3ChangedCount ()
             << endl;
      }
    }

    win_cnt [board.PlayoutWinner ()] ++;
    move_count += board.MoveCount();
  }

  // Report and regression checks.
  cerr
    << "sampler_test ok: "
    << win_cnt [Player::Black ()] << "/"
    << win_cnt [Player::White ()] << " "
    << move_count << " "
    << hash_changed_count << " "
    << endl;


  if (board_size == 9) {
    CHECK (win_cnt [Player::Black()] == 4473);
    CHECK (win_cnt [Player::White()] == 5527);
    CHECK (move_count  == move_count2);
    CHECK (move_count2 == 1109768);
    CHECK (hash_changed_count == 3703030);
  } else if (board_size == 19) {
    CHECK (win_cnt [Player::Black()] == 460);
    CHECK (win_cnt [Player::White()] == 540);
    CHECK (move_count  == move_count2);
    CHECK (move_count2 == 454316);
    CHECK (hash_changed_count == 1689165);
  } else {
    CHECK (false);
  }
}
