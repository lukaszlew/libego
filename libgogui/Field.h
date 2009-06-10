#ifndef FIELD_H
#define FIELD_H

#include <QGraphicsItemGroup>
#include <QPen>

class Field: public QGraphicsItemGroup
{

public:
  Field(int x, int y, QGraphicsItem *parent = 0);

  virtual void initializeBackgroudItem() = 0;

  virtual qreal getWidth() const = 0;
  virtual qreal getHeight() const = 0;

  int x() const { return m_x; }
  int y() const { return m_y; }

  enum EStoneColor
  {
    StoneWhite, StoneBlack
  };
  virtual QGraphicsItem* addStone(EStoneColor stoneColor);
  void removeStone();

  virtual QGraphicsItem* addMark();
  void removeMark();

  virtual QGraphicsItem* addCircle();
  void removeCircle();

  virtual QGraphicsItem* addSquare();
  void removeSquare();

  virtual QGraphicsItem* addTriangle();
  void removeTriangle();

  virtual QGraphicsItem* addLabel(const QString& label);
  void removeLabel();

  virtual QPainterPath shape () const { return m_background->shape(); }

protected:
  ///* debugging
  virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
  virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
  //*/
  virtual void removeItem(QGraphicsItem*& item);

  int m_x, m_y;


  QAbstractGraphicsShapeItem *m_background;
  QGraphicsItem *m_stone;
  QGraphicsItem *m_mark;
  QGraphicsItem *m_circle;
  QGraphicsItem *m_square;
  QGraphicsItem *m_triangle;
  QGraphicsItem *m_label;


  static const QString s_blackStoneFilename;
  static const QString s_whiteStoneFilename;

  static const qreal s_stoneAspectRatio;
  static const qreal s_shapeAspectRatio;

  static const QPen s_shapePen;

  static const qreal s_fieldZValue, s_stoneZValue, s_backgroundZValue, s_shapeZValue, s_labelZValue;


};

#endif // FIELD_H
