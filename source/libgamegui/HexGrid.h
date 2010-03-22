#ifndef HEXGRID_H_
#define HEXGRID_H_

#include "Grid.h"

class HexGrid: public Grid
{
public:
  HexGrid(int size, QGraphicsItem * parent = 0) : Grid(size, parent) {}

  virtual QRectF boundingRect() const;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *style, QWidget *widget);

  virtual QList<QPair<int,int> > getHandicapCoordinates() const;

  virtual int minimalXCoordinate(int y) const { return 1; }
  virtual int maximalXCoordinate(int y) const { return m_size; }

  virtual int minimalYCoordinate(int x) const { return 1; }
  virtual int maximalYCoordinate(int x) const { return m_size; }

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

#endif /* HEXGRID_H_ */
