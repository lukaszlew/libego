#ifndef HEXBOARD_H
#define HEXBOARD_H

#include "BoardScene.h"

class HexBoard: public BoardScene
{
Q_OBJECT

public:
  HexBoard(int size, QObject *parent = 0);

  virtual QGraphicsItem* addGrid();
  virtual QGraphicsItem* addRuler();

  static QPointF getFieldPosition(int x, int y);
};

#endif // HEXBOARD_H
