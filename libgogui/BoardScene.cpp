#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <boost/bind.hpp>

#include "BoardScene.h"

#include "Field.h"
#include "Grid.h"
#include "Ruler.h"

BoardScene::BoardScene(Grid* grid, QObject *parent) :
  QGraphicsScene(parent), m_grid(grid), m_ruler(m_grid->createRuler()) {
  setBackgroundBrush(QPixmap(":/images/wood.png"));

  addItem(m_grid);
  for (int x = m_grid->minimalXCoordinate(); x <= m_grid->maximalXCoordinate(); x++)
    for (int y = m_grid->minimalYCoordinate(x); y <= m_grid->maximalYCoordinate(x); y++)
      if (m_grid->isValidCoordinate(x, y)) {
        Field *field = m_grid->createField();
        field->setMousePressHandler(boost::bind(&BoardScene::handleMousePress, this, x, y, _1));
        m_fields.insert(getFieldString(x, y), field);
        field->setPos(m_grid->getFieldPosition(x, y));
        addItem(field);
      }
}

void BoardScene::addShape(int x, int y, EShapeType type, const QString& label) {
  QString pos = getFieldString(x, y);
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

void BoardScene::removeShape(int x, int y, EShapeType type) {
  QString pos = getFieldString(x, y);
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

void BoardScene::addBlackStone(int x, int y) {
  return addShape(x, y, TypeBlackStone);
}

void BoardScene::addWhiteStone(int x, int y) {
  return addShape(x, y, TypeWhiteStone);
}

void BoardScene::removeStone(int x, int y) {
  removeShape(x, y, TypeBlackStone);
}

void BoardScene::addMark(int x, int y) {
  return addShape(x, y, TypeMark);
}

void BoardScene::removeMark(int x, int y) {
  removeShape(x, y, TypeMark);
}

void BoardScene::addCircle(int x, int y) {
  return addShape(x, y, TypeCircle);
}

void BoardScene::removeCircle(int x, int y) {
  removeShape(x, y, TypeCircle);
}

void BoardScene::addSquare(int x, int y) {
  return addShape(x, y, TypeSquare);
}

void BoardScene::removeSquare(int x, int y) {
  removeShape(x, y, TypeSquare);
}

void BoardScene::addTriangle(int x, int y) {
  return addShape(x, y, TypeTriangle);
}

void BoardScene::removeTriangle(int x, int y) {
  removeShape(x, y, TypeTriangle);
}

void BoardScene::addLabel(int x, int y, const QString& label) {
  return addShape(x, y, TypeLabel, label);
}

void BoardScene::removeLabel(int x, int y) {
  removeShape(x, y, TypeLabel);
}

QString BoardScene::getFieldString(int x, int y) {
  QString res;
  res += QString::number(x);
  res += '-';
  res += QString::number(y);
  return res;
}

void BoardScene::handleMousePress(int x, int y, Qt::MouseButtons buttons) {
  if (buttons & Qt::LeftButton)
    addBlackStone(x, y);
  if (buttons & Qt::RightButton)
    addWhiteStone(x, y);
  if (buttons & Qt::MidButton)
    removeStone(x, y);
  if (buttons & Qt::XButton1)
    addMark(x, y);
  if (buttons & Qt::XButton2)
    removeMark(x, y);
}
