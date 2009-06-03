#ifndef HEXBOARD_H
#define HEXBOARD_H

#include "Board.h"

class HexBoard: public Board
{
Q_OBJECT

public:
  HexBoard(int size, QWidget *parent = 0);
  ~HexBoard();

private:

};

#endif // HEXBOARD_H
