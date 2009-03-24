#ifndef _BASIC_GTP_
#define _BASIC_GTP_

class BasicGtp : public GtpCommand {

public:
  BasicGtp (Gtp& gtp, Board& board_);
  virtual GtpResult exec_command (const string& command, istream& params); 

private:
  Board& board;
};

#endif
