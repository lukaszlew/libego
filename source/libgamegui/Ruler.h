#ifndef RULER_H_
#define RULER_H_

#include <QGraphicsItem>

class Grid;

class Ruler: public QGraphicsItem
{
public:
  enum ERulerType
  {
    LocateBefore = 0x1,
    LocateAfter = 0x2,
    TypeLetters = 0x4,
  };

  Ruler(int verticalType, int horizontalType, Grid* /*not-null */ grid);

  virtual QRectF boundingRect() const;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

  static QString getCoordinateString(int type, int coord);
private:
  const Grid *m_grid;
  const int m_verticalType;
  const int m_horizontalType;

  static const int s_fontSize;
};

#endif /* RULER_H_ */
