#ifndef _GENMOVE_GTP_H_
#define _GENMOVE_GTP_H_

#include "gtp.h"
#include "full_board.h"
#include "testing.h"

template <typename Engine>
class GenmoveGtp : public GtpCommand {
public:
  GenmoveGtp (Gtp& gtp, FullBoard& board, Engine& engine)
    : gtp_ (gtp), board_ (board), engine_ (engine)
  {
    gtp.add_gtp_command (this, "genmove");
  } 

  virtual GtpResult exec_command (const string& command, istream& params) {

    if (command == "genmove") {
      Player  player;
      Vertex   v;
      if (!(params >> player)) return GtpResult::syntax_error ();

      board_.set_act_player(player);
      //Engine* engine = new Engine (gtp_, board_);
      // TODO we don't need to allocate each time
      v = engine_.genmove ();
      //delete engine;

      if (v != Vertex::resign () &&
          board_.try_play (player, v) == false) {
        assert(false);
      }

      return GtpResult::success (v.to_string());
    }

    assert(false);
  } 

private:
  Gtp&       gtp_;
  FullBoard& board_;
  Engine&    engine_;
};

#endif
