#include <QGraphicsRectItem>
#include <QBrush>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsSvgItem>

#include "Field.h"

const QString Field::s_blackStoneFilename(":/images/gogui-black.svg");
const QString Field::s_whiteStoneFilename(":/images/gogui-white.svg");

const qreal Field::s_stoneAspectRatio = 1.0;
const qreal Field::s_shapeAspectRatio = 0.5;

const QPen Field::s_shapePen(Qt::red, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

const qreal Field::s_fieldZValue = 1.0;
const qreal Field::s_stoneZValue = 1.0;
const qreal Field::s_backgroundZValue = 0.0;
const qreal Field::s_shapeZValue = 2.0;
const qreal Field::s_labelZValue = 3.0;

Field::Field(int x, int y, QGraphicsItem *parent) :
  QGraphicsItemGroup(parent), m_x(x), m_y(y), m_background(NULL), m_stone(NULL), m_mark(NULL),
      m_circle(NULL), m_square(NULL), m_triangle(NULL), m_label(NULL) {
  setAcceptHoverEvents(true);
  setZValue(s_fieldZValue);
}

///* debugging functions
void Field::hoverEnterEvent ( QGraphicsSceneHoverEvent * event)
{
  Q_UNUSED(event);
  m_background->setBrush(QColor(64,64,64,64));
  update();
}

void Field::hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )
{
  Q_UNUSED(event);
  m_background->setBrush(Qt::NoBrush);
  update();
}
//*/

void Field::removeItem(QGraphicsItem*& item) {
  if (item) {
    scene()->removeItem(item);
    removeFromGroup(item);
    delete item;
    item = NULL;
  }
}

QGraphicsItem* Field::addStone(EStoneColor stoneColor) {
  removeStone();
  switch (stoneColor) {
  case StoneBlack:
    m_stone = new QGraphicsSvgItem(s_blackStoneFilename, this);
    break;
  case StoneWhite:
    m_stone = new QGraphicsSvgItem(s_whiteStoneFilename, this);
    break;
  default:
    break;
  }

  if (m_stone) {
    int max = (getWidth() > getHeight()) ? getWidth() : getHeight();
    qreal scaleRatio = s_stoneAspectRatio * max / m_stone->boundingRect().width();

    addToGroup(m_stone);

    m_stone->setZValue(s_stoneZValue);
    m_stone->scale(scaleRatio, scaleRatio);
    m_stone->translate(-m_stone->boundingRect().width() / 2, -m_stone->boundingRect().height() / 2);
  }

  return m_stone;
}

void Field::removeStone() {
  removeItem(m_stone);
}

class Mark: public QGraphicsRectItem
{
public:
  Mark(QGraphicsItem * parent = 0) :
    QGraphicsRectItem(parent) {
  }
  Mark(const QRectF & rect, QGraphicsItem * parent = 0) :
    QGraphicsRectItem(rect, parent) {
  }
  ;
  Mark(qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent = 0) :
    QGraphicsRectItem(x, y, width, height, parent) {
  }
  ~Mark() {
  }

  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    painter->setPen(pen());
    painter->drawLine(rect().bottomLeft(), rect().topRight());
    painter->drawLine(rect().topLeft(), rect().bottomRight());
  }
};

QGraphicsItem* Field::addMark() {
  Mark* mark;
  removeMark();
  qreal width = s_shapeAspectRatio * ((getWidth() > getHeight()) ? getWidth() : getHeight());
  m_mark = mark = new Mark(-width / 2, -width / 2, width, width, this);
  mark->setPen(s_shapePen);
  mark->setZValue(s_shapeZValue);
  addToGroup(m_mark);
  return m_mark;
}

void Field::removeMark() {
  removeItem(m_mark);
}

QGraphicsItem* Field::addCircle() {
  QGraphicsEllipseItem *circle;
  removeCircle();
  qreal width = s_shapeAspectRatio * ((getWidth() > getHeight()) ? getWidth() : getHeight());
  m_circle = circle = new QGraphicsEllipseItem(- width / 2, - width / 2, width, width, this);

  circle->setPen(s_shapePen);
  circle->setZValue(s_shapeZValue);

  addToGroup(m_circle);
  return m_circle;
}

void Field::removeCircle() {
  removeItem(m_circle);
}

QGraphicsItem* Field::addSquare() {
  QGraphicsRectItem *square;
  removeSquare();
  qreal width = s_shapeAspectRatio * ((getWidth() > getHeight()) ? getWidth() : getHeight());
  m_square = square = new QGraphicsRectItem(- width / 2, - width / 2, width, width, this);
  square->setPen(s_shapePen);
  square->setZValue(s_shapeZValue);
  addToGroup(m_square);
  return m_square;
}

void Field::removeSquare() {
  removeItem(m_square);
}

QGraphicsItem* Field::addTriangle() {
  QGraphicsPolygonItem *triangle;
  removeTriangle();
  qreal width = s_shapeAspectRatio * ((getWidth() > getHeight()) ? getWidth() : getHeight());
  QPolygonF polygon(3);
  polygon[0] = QPointF(-width / 2, width / 2);
  polygon[1] = QPointF(width / 2, width / 2);
  polygon[2] = QPointF(0.0, -width / 2);
  m_triangle = triangle = new QGraphicsPolygonItem(polygon, this);
  triangle->setPen(s_shapePen);
  triangle->setZValue(s_shapeZValue);
  addToGroup(m_triangle);
  return m_triangle;
}

void Field::removeTriangle() {
  removeItem(m_triangle);
}

QGraphicsItem* Field::addLabel(const QString& labelString) {

  QGraphicsSimpleTextItem *label;
  removeLabel();
  m_label = label = new QGraphicsSimpleTextItem(labelString, this);
  label->setPen(s_shapePen);
  addToGroup(m_label);
  label->setZValue(s_labelZValue);
  int max = (getWidth() > getHeight()) ? getWidth() : getHeight();
  qreal scaleRatio = s_shapeAspectRatio * max / m_label->boundingRect().height();
  m_label->scale(scaleRatio, scaleRatio);
  m_label->translate(-m_label->boundingRect().width() / 2, -m_label->boundingRect().height() / 2);
  return m_label;
}

void Field::removeLabel() {
  removeItem(m_label);
}
