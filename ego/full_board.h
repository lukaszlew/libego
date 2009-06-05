#ifndef _FULL_BOARD_H_
#define _FULL_BOARD_H_

#include "board.h"

class FullBoard : public Board {
public:
  const Board& board() const;
};

#endif
