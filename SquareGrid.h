#ifndef SQUAREGRID_H_
#define SQUAREGRID_H_

#include "Grid.h"

class SquareGrid: public Grid
{
public:
  SquareGrid(int size, QGraphicsItem * parent = 0);
  virtual ~SquareGrid();

  enum
  {
    Type = UserType + 1
  };

  virtual QRectF boundingRect() const;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
      QWidget *widget);

};

#endif /* SQUAREGRID_H_ */
