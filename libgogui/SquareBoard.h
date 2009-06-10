#ifndef SQUAREBOARD_H
#define SQUAREBOARD_H

#include "BoardScene.h"

class SquareBoard: public BoardScene
{
Q_OBJECT

public:
  SquareBoard(int size, QObject *parent = 0);

  static QPointF getFieldPosition(int x, int y);

protected:
  virtual QGraphicsItem* addGrid();
  virtual QGraphicsItem* addRuler();
};

#endif // SQUAREBOARD_H
