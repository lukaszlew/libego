#ifndef SQUAREBOARD_H
#define SQUAREBOARD_H

#include "Board.h"

class SquareBoard: public Board
{
Q_OBJECT

public:
  SquareBoard(int size, QWidget *parent = 0);
  ~SquareBoard();

private:

};

#endif // SQUAREBOARD_H
