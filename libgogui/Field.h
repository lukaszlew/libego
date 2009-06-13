#ifndef FIELD_H
#define FIELD_H

#include <QGraphicsItemGroup>
#include <QPen>
#include <boost/function.hpp>

class Field: public QGraphicsItemGroup
{

public:
  Field(const QPainterPath & path, QGraphicsItem *parent = 0);

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

  virtual QPainterPath shape() const {
    return m_background->shape();
  }

  typedef boost::function<void (Qt::MouseButtons buttons)> callback_type;
  void setMousePressHandler(const callback_type& handler);

protected:
  virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
  ///* debugging
  virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
  virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
  //*/
  virtual void removeItem(QGraphicsItem*& item);

  QAbstractGraphicsShapeItem *m_background;
  QGraphicsItem *m_stone;
  QGraphicsItem *m_mark;
  QGraphicsItem *m_circle;
  QGraphicsItem *m_square;
  QGraphicsItem *m_triangle;
  QGraphicsItem *m_label;
  callback_type m_mousePressHandler;

  static const QString s_blackStoneFilename;
  static const QString s_whiteStoneFilename;

  static const qreal s_stoneAspectRatio;
  static const qreal s_shapeAspectRatio;

  static const QPen s_shapePen;

  static const qreal s_fieldZValue, s_stoneZValue, s_backgroundZValue, s_shapeZValue, s_labelZValue;
};

#endif // FIELD_H
