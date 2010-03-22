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
  void drawRightDiagonalLines(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) const;
  void drawLeftDiagonalLines(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) const;
  void drawHandicapSpots(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) const;
  void drawFieldsShape(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) const;

  virtual QList<QPair<int,int> > getHandicapCoordinates() const = 0;

  virtual int minimalXCoordinate(int y) const = 0;
  virtual int maximalXCoordinate(int y) const = 0;

  virtual int minimalYCoordinate(int x) const = 0;
  virtual int maximalYCoordinate(int x) const = 0;

  virtual int minimalXCoordinate() const = 0;
  virtual int maximalXCoordinate() const = 0;

  virtual int minimalYCoordinate() const = 0;
  virtual int maximalYCoordinate() const = 0;

  virtual bool isValidCoordinate(int x, int y) const;

  virtual QPointF getFieldPosition(int x, int y) const = 0;

  virtual QPainterPath getPath() const = 0;
  virtual Ruler* createRuler() = 0;

  Field* createField(QGraphicsItem * parent = 0);

  virtual QPointF fieldSize() const = 0;

protected:
  int m_size;
  static const qreal s_gridZVlaue;
  static const qreal s_handicapSpotRadius;
};

#endif /* GRID_H_ */
