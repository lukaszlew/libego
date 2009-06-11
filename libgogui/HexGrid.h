#ifndef HEXGRID_H_
#define HEXGRID_H_

#include "Grid.h"

class HexGrid: public Grid
{
public:
  HexGrid(int size, QGraphicsItem * parent = 0);

  virtual QRectF boundingRect() const { return m_rect; }
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

  virtual QList<QPair<int,int> > getHandicapCoordinates() const;

  virtual int minimalCoordinate(int x) const;
  virtual int maximalCoordinate(int x) const;

  virtual int minimalCoordinate() const { return 1; }
  virtual int maximalCoordinate() const { return m_size; }

  virtual Field* createField(int x, int y, QGraphicsItem *parent = 0) const;

protected:
  virtual QPointF getFieldPosition(int x, int y) const;

  QRectF m_rect;
};

#endif /* HEXGRID_H_ */
