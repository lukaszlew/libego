#ifndef _GENMOVE_GTP_H_
#define _GENMOVE_GTP_H_

#include "gtp.h"
#include "board.h"
#include "config.h"

template <typename engine_t>
class GenmoveGtp : public GtpCommand {
public:
  GenmoveGtp (Gtp& gtp, Board& board_, engine_t& engine_) : board (board_) { //, engine (engine_)
    unused (engine_);
    gtp.add_gtp_command (this, "genmove");
  } 

  virtual GtpResult exec_command (const string& command, istream& params) {

    if (command == "genmove") {
      Player  player;
      Vertex   v;
      if (!(params >> player)) return GtpResult::syntax_error ();
  
      engine_t* engine = new engine_t (board); 
      // TODO we don't need to allocate each time
      v = engine->genmove (player);
      delete engine;

      if (v != Vertex::resign () &&
          board.try_play (player, v) == false) {
        assert(false);
      }

      return GtpResult::success (v.to_string());
    }

    assert(false);
  } 

private:
  Board& board;
};

#endif
