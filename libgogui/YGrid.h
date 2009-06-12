#ifndef YGRID_H_
#define YGRID_H_

#include "HexGrid.h"

class YGrid: public HexGrid
{
public:
  YGrid(int size, QGraphicsItem * parent = 0);

  virtual QRectF boundingRect() const;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *style, QWidget *widget);

  virtual int minimalXCoordinate(int y) const { return m_size - y + 1; }
  virtual int maximalXCoordinate(int y) const { return m_size; }

  virtual int minimalYCoordinate(int x) const { return m_size - x + 1; }
  virtual int maximalYCoordinate(int x) const { return m_size; }

  virtual int minimalXCoordinate() const { return 1; }
  virtual int maximalXCoordinate() const { return m_size; }

  virtual int minimalYCoordinate() const { return 1; }
  virtual int maximalYCoordinate() const { return m_size; }

  Ruler* createRuler();


};

#endif /* YGRID_H_ */
