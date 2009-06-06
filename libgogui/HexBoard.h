#ifndef HEXBOARD_H
#define HEXBOARD_H

#include "BoardScene.h"

class HexBoard: public BoardScene
{
Q_OBJECT

public:
  HexBoard(int size, QObject *parent = 0);
  ~HexBoard();

  virtual QGraphicsItem* addGrid();
  virtual QGraphicsItem* addRuler();


private:

};

#endif // HEXBOARD_H
