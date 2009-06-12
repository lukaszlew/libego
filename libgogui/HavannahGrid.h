#ifndef HAVANNAHGRID_H_
#define HAVANNAHGRID_H_

#include "Grid.h"

class HavannahGrid : public Grid
{
public:
  HavannahGrid(int size, QGraphicsItem * parent = 0) : Grid(size, parent) {}

  virtual QRectF boundingRect() const;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *style, QWidget *widget);

  virtual QList<QPair<int,int> > getHandicapCoordinates() const;

  virtual int minimalXCoordinate(int y) const;
  virtual int maximalXCoordinate(int y) const;

  virtual int minimalYCoordinate(int x) const;
  virtual int maximalYCoordinate(int x) const;

  virtual int minimalXCoordinate() const { return 1; }
  virtual int maximalXCoordinate() const { return m_size; }

  virtual int minimalYCoordinate() const { return 1; }
  virtual int maximalYCoordinate() const { return m_size; }

  virtual QPainterPath getPath() const;
  virtual Ruler* createRuler();

  virtual QPointF fieldSize() const { return QPointF(s_width, s_height); }

protected:
  virtual QPointF getFieldPosition(int x, int y) const;

  static const qreal s_width, s_height;
};

#endif /* HAVANNAHGRID_H_ */
