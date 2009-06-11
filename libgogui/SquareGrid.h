#ifndef SQUAREGRID_H_
#define SQUAREGRID_H_

#include "Grid.h"

class SquareGrid: public Grid
{
public:
  SquareGrid(int size, QGraphicsItem * parent = 0) : Grid(size, parent) {}

  virtual QRectF boundingRect() const;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

  virtual QList<QPair<int,int> > getHandicapCoordinates() const;

  virtual int minimalCoordinate(int x) const { return 1; }
  virtual int maximalCoordinate(int x) const { return m_size; }

  virtual int minimalCoordinate() const { return 1; }
  virtual int maximalCoordinate() const { return m_size; }

  virtual Field* createField(int x, int y, QGraphicsItem *parent = 0) const;

protected:
  virtual QPointF getFieldPosition(int x, int y) const;
};

#endif /* SQUAREGRID_H_ */
