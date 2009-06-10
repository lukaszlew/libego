#ifndef HEXBOARD_H
#define HEXBOARD_H

#include "BoardScene.h"

class HexBoard: public BoardScene
{
Q_OBJECT

public:
  HexBoard(int size, QObject *parent = 0);

  static QPointF getFieldPosition(int x, int y);

protected:
  virtual QGraphicsItem* addGrid();
  virtual QGraphicsItem* addRuler();
};

#endif // HEXBOARD_H
