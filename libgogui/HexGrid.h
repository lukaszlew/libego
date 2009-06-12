#ifndef HEXGRID_H_
#define HEXGRID_H_

#include "Grid.h"

class HexGrid: public Grid
{
public:
  HexGrid(int size, QGraphicsItem * parent = 0);

  virtual QRectF boundingRect() const { return m_rect; }
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *style, QWidget *widget);

  virtual QList<QPair<int,int> > getHandicapCoordinates() const;

  virtual int minimalXCoordinate(int y) const;
  virtual int maximalXCoordinate(int y) const;

  virtual int minimalYCoordinate(int x) const;
  virtual int maximalYCoordinate(int x) const;

  virtual int minimalCoordinate() const { return 1; }
  virtual int maximalCoordinate() const { return m_size; }

  virtual QPainterPath getPath() const;
  virtual Ruler* createRuler();

protected:
  virtual QPointF getFieldPosition(int x, int y) const;

  QRectF m_rect;

  static const qreal s_width, s_height;
};

#endif /* HEXGRID_H_ */
