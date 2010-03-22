#ifndef HOGRID_H_
#define HOGRID_H_

#include "HexGrid.h"

class HoGrid: public HexGrid
{
public:
  HoGrid(int size, QGraphicsItem * parent = 0) : HexGrid(size, parent) {}

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


  virtual Ruler* createRuler();
};

#endif /* HOGRID_H_ */
