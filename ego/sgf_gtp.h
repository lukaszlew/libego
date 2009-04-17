#ifndef _SGF_GTP_H_
#define _SGF_GTP_H_

#include "board.h"
#include "gtp.h"
#include "sgf.h"

class SgfGtp : public GtpCommand {
public:
  SgfGtp (Gtp& _gtp, SgfTree& _sgf_tree, Board& _base_board);
  virtual GtpResult exec_command (const string& command, istream& params);
  void exec_embedded_gtp_rec (SgfNode* current_node, ostream& response);
  
  SgfTree&  sgf_tree;
  Gtp&      gtp;
  Board&    base_board;

};

#endif
