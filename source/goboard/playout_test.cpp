#include "playout_test.hpp"

void PlayoutTest (bool print_moves) {
  Board empty;
  Board board;
  FastRandom random (123);
  NatMap <Player, uint> win_cnt (0);
  uint move_count = 0;
  uint move_count2 = 0;
  uint hash_changed_count = 0;

  rep (ii, 10000) {
    board.Load (empty);
    while (!board.BothPlayerPass ()) {
      move_count2 += 1;
      FastStack<Vertex, Board::kArea> legals; // TODO pass
      Player pl = board.ActPlayer();
      ForEachNat (Vertex, v) {
        IFNCHECK (board.ColorAt(v) != Color::Empty() ||
                  board.KoVertex() == v ||
                  board.IsLegal (pl, v) == board.Hash3x3At(v).IsLegal(pl), {
                    board.DebugPrint (v);
                    cerr << pl.ToGtpString () << " to play" << endl;
                  });
        if (v != Vertex::Pass () &&
            board.IsLegal (pl, v) &&
            !board.IsEyelike (pl, v)) {
          legals.Push(v);
        }
      }

      Vertex v;
      uint random_idx = 999;
      if (legals.Size() == 0) {
        v = Vertex::Pass ();
      } else {
        random_idx = random.GetNextUint (legals.Size());
        v = legals.Remove (random_idx);
      }

      board.PlayLegal (pl, v);
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

  cerr
    << "board_test ok: "
    << win_cnt [Player::Black ()] << "/"
    << win_cnt [Player::White ()] << " "
    << move_count << " "
    << hash_changed_count << " "
    << endl;

  CHECK (win_cnt [Player::Black()] == 4448);
  CHECK (win_cnt [Player::White()] == 5552);
  CHECK (move_count  == move_count2);
  CHECK (move_count2 == 1115760);
  CHECK (hash_changed_count == 10002578);
}
