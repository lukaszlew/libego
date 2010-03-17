#ifndef FIELD_H
#define FIELD_H

#include <QGraphicsItemGroup>
#include <QPen>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

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

  virtual void addBGMark(const QColor& c = QColor(32,32,32,32));
  void removeBGMark();

  virtual QPainterPath shape() const {
    return m_background->shape();
  }

  virtual void clearField();

  typedef boost::function<void (Qt::MouseButtons buttons)> mouse_callback_type;
  typedef boost::function<void (int delta)> wheel_callback_type;
  typedef boost::function<void ()> hoover_callback_type;
  void setMousePressHandler(const mouse_callback_type& handler) { m_mousePressHandler = handler; }
  void setMouseDoubleClickHandler(const mouse_callback_type& handler) { m_mouseDoubleClickHandler = handler; }
  void setMouseReleaseHandler(const mouse_callback_type& handler) { m_mouseReleaseHandler = handler; }
  void setMouseWheelHandler(const wheel_callback_type& handler) { m_mouseWheelHandler = handler; }
  void setHooverEnterHandler(const hoover_callback_type& handler) { m_hooverEnterHandler = handler; }
  void setHooverLeaveHandler(const hoover_callback_type& handler) { m_hooverLeaveHandler = handler; }

protected:
  virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
  virtual void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event );
  virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
  virtual void wheelEvent ( QGraphicsSceneWheelEvent * event );
  virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
  virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );

  virtual void removeItem(QGraphicsItem*& item);

  QAbstractGraphicsShapeItem *m_background;
  QGraphicsItem *m_stone;
  QGraphicsItem *m_mark;
  QGraphicsItem *m_circle;
  QGraphicsItem *m_square;
  QGraphicsItem *m_triangle;
  QGraphicsItem *m_label;
  boost::shared_ptr<QColor> bgMark;

  mouse_callback_type m_mousePressHandler;
  mouse_callback_type m_mouseReleaseHandler;
  mouse_callback_type m_mouseDoubleClickHandler;
  wheel_callback_type m_mouseWheelHandler;
  hoover_callback_type m_hooverEnterHandler;
  hoover_callback_type m_hooverLeaveHandler;

  static const QString s_blackStoneFilename;
  static const QString s_whiteStoneFilename;

  static const qreal s_stoneAspectRatio;
  static const qreal s_shapeAspectRatio;

  static const QPen s_shapePen;

  static const qreal s_fieldZValue, s_stoneZValue, s_backgroundZValue, s_shapeZValue, s_labelZValue;
};

#endif // FIELD_H
