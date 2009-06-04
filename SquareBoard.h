#ifndef SQUAREBOARD_H
#define SQUAREBOARD_H

#include "BoardScene.h"

class SquareBoard: public BoardScene
{
Q_OBJECT

public:
  SquareBoard(int size, QObject *parent = 0);
  virtual ~SquareBoard();

  virtual QGraphicsItem* addGrid();
  virtual QGraphicsItem* addRuler();

private:

};

#endif // SQUAREBOARD_H
