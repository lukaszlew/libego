#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <boost/bind.hpp>

#include "BoardScene.h"

#include "Field.h"
#include "Grid.h"
#include "Ruler.h"

#include "SquareGrid.h"
#include "HexGrid.h"
#include "HoGrid.h"
#include "HavannahGrid.h"
#include "YGrid.h"

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

BoardScene *BoardScene::createHavannahScene(int size, QObject *parent) {
  return new BoardScene(new HavannahGrid(size), parent);
}

BoardScene *BoardScene::createHexScene(int size, QObject *parent) {
  return new BoardScene(new HexGrid(size), parent);
}

BoardScene *BoardScene::createHoScene(int size, QObject *parent) {
  return new BoardScene(new HoGrid(size), parent);
}

BoardScene *BoardScene::createGoScene(int size, QObject *parent) {
  return new BoardScene(new SquareGrid(size), parent);
}

BoardScene *BoardScene::createYScene(int size, QObject *parent) {
  return new BoardScene(new YGrid(size), parent);
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
  return QString::number(x) + '-' + QString::number(y);
}

void BoardScene::handleMousePress(int x, int y, Qt::MouseButtons buttons) {
  if (buttons & Qt::LeftButton && m_LeftButtonHandler) m_LeftButtonHandler(x, y);
  if (buttons & Qt::RightButton && m_rightButtonHandler) m_rightButtonHandler(x, y);
  if (buttons & Qt::MidButton && m_midButtonHandler) m_midButtonHandler(x, y);
  if (buttons & Qt::XButton1 && m_xButton1Handler) m_xButton1Handler(x, y);
  if (buttons & Qt::XButton2 && m_xButton2Handler) m_xButton2Handler(x, y);
}
