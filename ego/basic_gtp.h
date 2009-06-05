#ifndef _BASIC_GTP_
#define _BASIC_GTP_

#include "gtp.h"
#include "full_board.h"

class BasicGtp : public GtpCommand {

public:
  BasicGtp (Gtp& gtp, FullBoard& board_);
  virtual ~BasicGtp () {};
  virtual GtpResult exec_command (const string& command, istream& params); 

private:
  FullBoard& board;
};

#endif
