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

  int x() const { return m_x; }
  int y() const { return m_y; }

  enum
  {
    Type = UserType + 2
  };

  enum EStoneColor
  {
    StoneWhite, StoneBlack
  };
  virtual QGraphicsItem* addStone(EStoneColor stoneColor);
  virtual void removeStone();

  virtual QGraphicsItem* addMark(QColor color);
  virtual void removeMark();

  virtual QGraphicsItem* addCircle(QColor color);
  virtual void removeCircle();

  virtual QGraphicsItem* addSquare(QColor color);
  virtual void removeSquare();

  virtual QGraphicsItem* addTriangle(QColor color);
  virtual void removeTriangle();

  virtual QGraphicsItem* addLabel(const QString& label);
  virtual void removeLabel();

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
