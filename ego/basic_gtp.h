#ifndef _BASIC_GTP_
#define _BASIC_GTP_

#include "full_board.h"
#include "../libgtp/gtp.h"

class BasicGtp {

public:
  BasicGtp (Gtp::Repl& gtp, FullBoard& board_);

private:
  void CBoardsize (Gtp::Io& io);
  void CClear_board (Gtp::Io& io);
  void CKomi (Gtp::Io& io);
  void CPlay (Gtp::Io& io);
  void CUndo (Gtp::Io& io);
  void CShowboard (Gtp::Io& io);

private:
  FullBoard& board;
};

#endif
