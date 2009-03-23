#ifndef _GENMOVE_GTP_H_
#define _GENMOVE_GTP_H_

template <typename engine_t>
class GenmoveGtp : public GtpCommand {
public:
  GenmoveGtp (Gtp& gtp, Board& board_, engine_t& engine_) : board (board_) { //, engine (engine_)
    unused (engine_);
    gtp.add_gtp_command (this, "genmove");
  } 

  virtual GtpResult exec_command (string command, istream& params) {

    if (command == "genmove") {
      Player  player;
      Vertex   v;
      if (!(params >> player)) return GtpResult::syntax_error ();
  
      engine_t* engine = new engine_t (board); // TODO Why we need to allocate?
      v = engine->genmove (player);
      delete engine;

      if (v != Vertex::resign () &&
          board.try_play (player, v) == false) {
        fatal_error ("genmove: generated illegal move");
      }

      return GtpResult::success (to_string (v));
    }

    fatal_error ("wrong command in GtpGenmove::exec_command");
    assert (false);
  } 

private:
  Board& board;
};

#endif
