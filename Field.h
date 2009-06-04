#ifndef FIELD_H
#define FIELD_H

#include <QGraphicsItemGroup>

class Field: public QGraphicsItemGroup
{

public:
  Field(int x, int y, QGraphicsItem *parent = 0);
  ~Field();

  virtual void initializeBackgroudItem() = 0;

  virtual int getWidth() const = 0;
  virtual int getHeight() const = 0;

  enum EStoneColor
  {
    StoneWhite, StoneBlack
  };
  QGraphicsItem* addStone(EStoneColor stoneColor);
  void removeStone();

  QGraphicsItem* addTriangle(QColor color);
  void removeTriangle();

protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);

  int m_x, m_y;
  QAbstractGraphicsShapeItem *m_background;
  QAbstractGraphicsShapeItem *m_triangle;
  QGraphicsItem *m_stone;

  static const QString s_blackStoneFilename;
  static const QString s_whiteStoneFilename;

  static const qreal s_stoneAspectRatio;

};

#endif // FIELD_H
