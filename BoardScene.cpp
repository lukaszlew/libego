#include <QGraphicsSceneMouseEvent>
#include "BoardScene.h"

#include "Field.h"

BoardScene::BoardScene(int size, QObject *parent)
: QGraphicsScene(parent), m_size(size)
{
  setBackgroundBrush(QPixmap(":/images/wood.png"));
}

BoardScene::~BoardScene()
{
}

QGraphicsItem* BoardScene::addBlackStone(const QString& pos)
{
  map_type::iterator it = m_fields.find(pos);
  if (it != m_fields.end()) {
    return (*it)->addStone(Field::StoneBlack);
  } else {
    return NULL;
  }
}

QGraphicsItem* BoardScene::addWhiteStone(const QString& pos)
{
  map_type::iterator it = m_fields.find(pos);
  if (it != m_fields.end()) {
    return (*it)->addStone(Field::StoneWhite);
  } else {
    return NULL;
  }
}

void BoardScene::removeStone(const QString& pos)
{
  map_type::iterator it = m_fields.find(pos);
  if (it != m_fields.end()) {
    return (*it)->removeStone();
  }
}

QGraphicsItem* BoardScene::addMark(const QString& pos, QColor color)
{
  map_type::iterator it = m_fields.find(pos);
  if (it != m_fields.end()) {
    return (*it)->addMark(color);
  } else {
    return NULL;
  }
}

void BoardScene::removeMark(const QString& pos)
{
  map_type::iterator it = m_fields.find(pos);
  if (it != m_fields.end()) {
    return (*it)->removeMark();
  }
}

QGraphicsItem* BoardScene::addCircle(const QString& pos, QColor color)
{
  map_type::iterator it = m_fields.find(pos);
  if (it != m_fields.end()) {
    return (*it)->addCircle(color);
  } else {
    return NULL;
  }
}

void BoardScene::removeCircle(const QString& pos)
{
  map_type::iterator it = m_fields.find(pos);
  if (it != m_fields.end()) {
    return (*it)->removeCircle();
  }
}

QGraphicsItem* BoardScene::addSquare(const QString& pos, QColor color)
{
  map_type::iterator it = m_fields.find(pos);
  if (it != m_fields.end()) {
    return (*it)->addSquare(color);
  } else {
    return NULL;
  }
}

void BoardScene::removeSquare(const QString& pos)
{
  map_type::iterator it = m_fields.find(pos);
  if (it != m_fields.end()) {
    return (*it)->removeSquare();
  }
}

QGraphicsItem* BoardScene::addTriangle(const QString& pos, QColor color)
{
  map_type::iterator it = m_fields.find(pos);
  if (it != m_fields.end()) {
    return (*it)->addTriangle(color);
  } else {
    return NULL;
  }
}

void BoardScene::removeTriangle(const QString& pos)
{
  map_type::iterator it = m_fields.find(pos);
  if (it != m_fields.end()) {
    return (*it)->removeTriangle();
  }
}

QGraphicsItem* BoardScene::addLabel(const QString& pos, const QString& label)
{
  map_type::iterator it = m_fields.find(pos);
  if (it != m_fields.end()) {
    return (*it)->addLabel(label);
  } else {
    return NULL;
  }
}

void BoardScene::removeLabel(const QString& pos)
{
  map_type::iterator it = m_fields.find(pos);
  if (it != m_fields.end()) {
    return (*it)->removeLabel();
  }
}

void BoardScene::removeRuler()
{
  if (m_ruler) {
    removeItem(m_ruler);
    delete m_ruler;
    m_ruler = NULL;
    update();
  }
}

QString BoardScene::getFieldString(int x, int y)
{
  QString res;
  res += QString::number(x);
  res += '-';
  res += QString::number(y);
  return res;
}

void BoardScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
  foreach (QGraphicsItem *item, items(mouseEvent->buttonDownScenePos(mouseEvent->button()))) {
    Field* field = qgraphicsitem_cast<Field*>(item);

    if (!field)
      continue;

    emit fieldClicked(getFieldString(field->x(), field->y()),
        mouseEvent->buttons());

  }

}
