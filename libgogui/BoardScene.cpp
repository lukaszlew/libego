#include <QGraphicsSceneMouseEvent>

#include "BoardScene.h"

#include "Field.h"
#include "Grid.h"
#include "Ruler.h"

BoardScene::BoardScene(Grid* grid, QObject *parent) :
  QGraphicsScene(parent), m_grid(grid), m_ruler(m_grid->createRuler()) {
  setBackgroundBrush(QPixmap(":/images/wood.png"));

  addItem(m_grid);
  for (int x = m_grid->minimalCoordinate(); x <= m_grid->maximalCoordinate(); x++) {
    for (int y = m_grid->minimalYCoordinate(x); y <= m_grid->maximalYCoordinate(x); y++) {
      Field *field = m_grid->createField(x, y);
      m_fields.insert(getFieldString(x, y), field);
      field->setPos(m_grid->getFieldPosition(x, y));
      addItem(field);
    }
  }
}

void BoardScene::addShape(const QString& pos, EShapeType type, const QString& label) {
  QMutexLocker locker(&m_mutex);

  map_type::iterator it = m_fields.find(pos);
  if (it != m_fields.end()) {
    switch (type) {
    case TypeBlackStone:
      (*it)->addStone(Field::StoneBlack);
      return;
    case TypeWhiteStone:
      (*it)->addStone(Field::StoneWhite);
      return;
    case TypeMark:
      (*it)->addMark();
      return;
    case TypeCircle:
      (*it)->addCircle();
      return;
    case TypeSquare:
      (*it)->addSquare();
      return;
    case TypeTriangle:
      (*it)->addTriangle();
      return;
    case TypeLabel:
      (*it)->addLabel(label);
      return;
    default:
      return;
    }
  }
}

void BoardScene::removeShape(const QString& pos, EShapeType type) {
  QMutexLocker locker(&m_mutex);

  map_type::iterator it = m_fields.find(pos);
  if (it != m_fields.end()) {
    switch (type) {
    case TypeBlackStone:
    case TypeWhiteStone:
      return (*it)->removeStone();
    case TypeMark:
      return (*it)->removeMark();
    case TypeCircle:
      return (*it)->removeCircle();
    case TypeSquare:
      return (*it)->removeSquare();
    case TypeTriangle:
      return (*it)->removeTriangle();
    case TypeLabel:
      return (*it)->removeLabel();
    default:
      return;
    }
  }
}

void BoardScene::addBlackStone(const QString& pos) {
  return addShape(pos, TypeBlackStone);
}

void BoardScene::addWhiteStone(const QString& pos) {
  return addShape(pos, TypeWhiteStone);
}

void BoardScene::removeStone(const QString& pos) {
  removeShape(pos, TypeBlackStone);
}

void BoardScene::addMark(const QString& pos) {
  return addShape(pos, TypeMark);
}

void BoardScene::removeMark(const QString& pos) {
  removeShape(pos, TypeMark);
}

void BoardScene::addCircle(const QString& pos) {
  return addShape(pos, TypeCircle);
}

void BoardScene::removeCircle(const QString& pos) {
  removeShape(pos, TypeCircle);
}

void BoardScene::addSquare(const QString& pos) {
  return addShape(pos, TypeSquare);
}

void BoardScene::removeSquare(const QString& pos) {
  removeShape(pos, TypeSquare);
}

void BoardScene::addTriangle(const QString& pos) {
  return addShape(pos, TypeTriangle);
}

void BoardScene::removeTriangle(const QString& pos) {
  removeShape(pos, TypeTriangle);
}

void BoardScene::addLabel(const QString& pos, const QString& label) {
  return addShape(pos, TypeLabel, label);
}

void BoardScene::removeLabel(const QString& pos) {
  removeShape(pos, TypeLabel);
}

QString BoardScene::getFieldString(int x, int y) {
  QString res;
  res += QString::number(x);
  res += '-';
  res += QString::number(y);
  return res;
}

void BoardScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent) {
  QList<QGraphicsItem *> itemList = items(mouseEvent->buttonDownScenePos(mouseEvent->button()));
  foreach (QGraphicsItem *item, itemList)
    {
      Field* field = qgraphicsitem_cast<Field*> (item);

      if (!field)
        continue;

      if (field->contains(field->mapFromScene(mouseEvent->buttonDownScenePos(mouseEvent->button()))))
        debugClick(getFieldString(field->getX(), field->getY()), mouseEvent->buttons());

    }
  QGraphicsScene::mousePressEvent(mouseEvent);
}

///*
void BoardScene::debugClick(const QString& pos, Qt::MouseButtons buttons) {
  if (buttons & Qt::LeftButton)
    addBlackStone(pos);
  if (buttons & Qt::RightButton)
    addWhiteStone(pos);
  if (buttons & Qt::MidButton)
    removeStone(pos);
  if (buttons & Qt::XButton1)
    addCircle(pos);
  if (buttons & Qt::XButton2)
    removeCircle(pos);

}
//*/
