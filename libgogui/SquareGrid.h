#ifndef SQUAREGRID_H_
#define SQUAREGRID_H_

#include "Grid.h"

class SquareGrid: public Grid
{
public:
  SquareGrid(int size, QGraphicsItem * parent = 0) : Grid(size, parent) {}

  virtual QRectF boundingRect() const;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *style, QWidget *widget);

  virtual QList<QPair<int,int> > getHandicapCoordinates() const;

  virtual int minimalXCoordinate(int ) const { return 1; }
  virtual int maximalXCoordinate(int ) const { return m_size; }

  virtual int minimalYCoordinate(int ) const { return 1; }
  virtual int maximalYCoordinate(int ) const { return m_size; }

  virtual int minimalCoordinate() const { return 1; }
  virtual int maximalCoordinate() const { return m_size; }

  virtual QPainterPath getPath() const;
  virtual Ruler* createRuler();

  virtual qreal fieldWidth() const { return s_width; }
  virtual qreal fieldHeight() const { return s_height; }

protected:
  virtual QPointF getFieldPosition(int x, int y) const;

  static const qreal s_width, s_height;
};

#endif /* SQUAREGRID_H_ */
