#ifndef HEXGRID_H_
#define HEXGRID_H_

#include "Grid.h"

class HexGrid: public Grid
{
public:
  HexGrid(int size, QGraphicsItem * parent = 0);
  virtual ~HexGrid();

  virtual QRectF boundingRect() const;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
      QWidget *widget);

};

#endif /* HEXGRID_H_ */
