#ifndef GRID_H_
#define GRID_H_

#include <QGraphicsItem>

#include <QPair>

class Field;
class Ruler;

class Grid: public QGraphicsItem
{
public:
  Grid(int size, QGraphicsItem * parent = 0);

  void drawHorizontalLines(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) const;
  void drawVerticalLines(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) const;
  void drawDiagonalLines(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) const;
  void drawHandicapSpots(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) const;

  virtual QList<QPair<int,int> > getHandicapCoordinates() const = 0;

  virtual int minimalCoordinate(int x) const = 0;
  virtual int maximalCoordinate(int x) const = 0;

  virtual int minimalCoordinate() const = 0;
  virtual int maximalCoordinate() const = 0;

  bool isValidCoordinate(int x, int y) const;

  virtual QPointF getFieldPosition(int x, int y) const = 0;

  virtual Field* createField(int x, int y, QGraphicsItem *parent = 0) const = 0;

protected:
  int m_size;
  static const qreal s_gridZVlaue;
  static const qreal s_handicapSpotRadius;
};

#endif /* GRID_H_ */
