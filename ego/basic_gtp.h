#ifndef _BASIC_GTP_
#define _BASIC_GTP_

#include "gtp.h"
#include "board.h"

class BasicGtp : public GtpCommand {

public:
  BasicGtp (Gtp& gtp, Board& board_);
  virtual GtpResult exec_command (const string& command, istream& params); 

private:
  Board& board;
};

#endif
