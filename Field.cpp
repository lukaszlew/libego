#include <QGraphicsRectItem>
#include <QBrush>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsSvgItem>
#include <QDebug>

#include "Field.h"

const QString Field::s_blackStoneFilename(":/images/gogui-black.svg");
const QString Field::s_whiteStoneFilename(":/images/gogui-white.svg");

const qreal Field::s_stoneAspectRatio = 1.0;

Field::Field(int x, int y, QGraphicsItem *parent) :
  QGraphicsItemGroup(parent), m_x(x), m_y(y), m_background(NULL), m_stone(NULL)
{
}

Field::~Field()
{
}

void Field::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  if (m_stone)
    removeStone();
  else {
    if (Qt::LeftButton & event->buttons())
      addStone(StoneBlack);
    else
      addStone(StoneWhite);
  }
}

QGraphicsItem* Field::addStone(EStoneColor stoneColor)
{
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
    qreal scaleRatio = s_stoneAspectRatio * max
        / m_stone->boundingRect().width();

    addToGroup(m_stone);

    m_stone->scale(scaleRatio, scaleRatio);
    m_stone->translate(-m_stone->boundingRect().width() / 2,
        -m_stone->boundingRect().height() / 2);
  }

  return m_stone;
}

void Field::removeStone()
{
  if (m_stone) {
    scene()->removeItem(m_stone);
    removeFromGroup(m_stone);
    delete m_stone;
    m_stone = NULL;
  }
}

QGraphicsItem* Field::addTriangle(QColor color)
{
  Q_UNUSED(color)
  //TODO
  return NULL;
}

void Field::removeTriangle()
{
  //TODO
}
