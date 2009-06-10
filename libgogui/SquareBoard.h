#ifndef SQUAREBOARD_H
#define SQUAREBOARD_H

#include "BoardScene.h"

class SquareBoard: public BoardScene
{
Q_OBJECT

public:
  SquareBoard(int size, QObject *parent = 0);

  virtual QGraphicsItem* addGrid();
  virtual QGraphicsItem* addRuler();

  static QPointF getFieldPosition(int x, int y);
};

#endif // SQUAREBOARD_H
