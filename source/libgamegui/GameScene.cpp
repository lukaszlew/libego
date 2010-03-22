#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <boost/bind.hpp>

#include "GameScene.h"

#include "Field.h"
#include "Grid.h"
#include "Ruler.h"

#include "SquareGrid.h"
#include "HexGrid.h"
#include "HoGrid.h"
#include "HavannahGrid.h"
#include "YGrid.h"

GameScene::GameScene(Grid* grid, QObject *parent) :
  QGraphicsScene(parent), m_grid(grid), m_ruler(m_grid->createRuler()) {
  setBackgroundBrush(QPixmap(":/images/wood.png"));

  addItem(m_grid);
  for (int x = m_grid->minimalXCoordinate(); x <= m_grid->maximalXCoordinate(); x++)
    for (int y = m_grid->minimalYCoordinate(x); y <= m_grid->maximalYCoordinate(x); y++)
      if (m_grid->isValidCoordinate(x, y)) {
        Field *field = m_grid->createField();
        field->setMousePressHandler(boost::bind(&GameScene::handleMousePress, this, x, y, _1));
        field->setMouseDoubleClickHandler(boost::bind(&GameScene::handleMouseDoubleClick, this, x, y, _1));
        field->setMouseReleaseHandler(boost::bind(&GameScene::handleMouseRelease, this, x, y, _1));
        field->setMouseWheelHandler(boost::bind(&GameScene::handleWheelMove, this, x, y, _1));
        field->setHooverEnterHandler(boost::bind(&GameScene::handleHooverEnter, this, x, y));
        field->setHooverLeaveHandler(boost::bind(&GameScene::handleHooverLeave, this, x, y));
        m_fields.insert(getFieldString(x, y), field);
        field->setPos(m_grid->getFieldPosition(x, y));
        addItem(field);
      }
}

void GameScene::clearBoard() {
    for (map_type::iterator it=m_fields.begin(); it!=m_fields.end(); ++it) {
      (*it)->clearField();
    }
}

GameScene *GameScene::createHavannahScene(int size, QObject *parent) {
  return new GameScene(new HavannahGrid(size), parent);
}

GameScene *GameScene::createHexScene(int size, QObject *parent) {
  return new GameScene(new HexGrid(size), parent);
}

GameScene *GameScene::createHoScene(int size, QObject *parent) {
  return new GameScene(new HoGrid(size), parent);
}

GameScene *GameScene::createGoScene(int size, QObject *parent) {
  return new GameScene(new SquareGrid(size, SquareGrid::ReverseY), parent);
}

GameScene *GameScene::createYScene(int size, QObject *parent) {
  return new GameScene(new YGrid(size), parent);
}

void GameScene::addShape(int x, int y, EShapeType type, const QString& label) {
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

void GameScene::removeShape(int x, int y, EShapeType type) {
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

void GameScene::addBlackStone(int x, int y) {
  return addShape(x, y, TypeBlackStone);
}

void GameScene::addWhiteStone(int x, int y) {
  return addShape(x, y, TypeWhiteStone);
}

void GameScene::removeStone(int x, int y) {
  removeShape(x, y, TypeBlackStone);
}

void GameScene::addMark(int x, int y) {
  return addShape(x, y, TypeMark);
}

void GameScene::removeMark(int x, int y) {
  removeShape(x, y, TypeMark);
}

void GameScene::addCircle(int x, int y) {
  return addShape(x, y, TypeCircle);
}

void GameScene::removeCircle(int x, int y) {
  removeShape(x, y, TypeCircle);
}

void GameScene::addSquare(int x, int y) {
  return addShape(x, y, TypeSquare);
}

void GameScene::removeSquare(int x, int y) {
  removeShape(x, y, TypeSquare);
}

void GameScene::addTriangle(int x, int y) {
  return addShape(x, y, TypeTriangle);
}

void GameScene::removeTriangle(int x, int y) {
  removeShape(x, y, TypeTriangle);
}

void GameScene::addLabel(int x, int y, const QString& label) {
  return addShape(x, y, TypeLabel, label);
}

void GameScene::removeLabel(int x, int y) {
  removeShape(x, y, TypeLabel);
}

void GameScene::addBGMark(int x, int y, const QColor& c) {
  QString pos = getFieldString(x, y);
  QMutexLocker locker(&m_mutex);

  map_type::iterator it = m_fields.find(pos);
  if (it != m_fields.end()) {
    (*it)->addBGMark(c);
  }
}

void GameScene::removeBGMark(int x, int y) {
  QString pos = getFieldString(x, y);
  QMutexLocker locker(&m_mutex);

  map_type::iterator it = m_fields.find(pos);
  if (it != m_fields.end()) {
    (*it)->removeBGMark();
  }
}

QString GameScene::getFieldString(int x, int y) {
  return QString::number(x) + '-' + QString::number(y);
}

/*
void GameScene::handleMousePress(int x, int y, Qt::MouseButtons buttons) {
  if (buttons & Qt::LeftButton && m_LeftButtonHandler) m_LeftButtonHandler(x, y);
  if (buttons & Qt::RightButton && m_rightButtonHandler) m_rightButtonHandler(x, y);
  if (buttons & Qt::MidButton && m_midButtonHandler) m_midButtonHandler(x, y);
  if (buttons & Qt::XButton1 && m_xButton1Handler) m_xButton1Handler(x, y);
  if (buttons & Qt::XButton2 && m_xButton2Handler) m_xButton2Handler(x, y);
}

//*/

void GameScene::handleMousePress(int x, int y, Qt::MouseButtons buttons) {
  emit mousePressed(x, y, buttons);
}

void GameScene::handleMouseDoubleClick(int x, int y, Qt::MouseButtons buttons) {
  emit mouseDoubleClicked(x, y, buttons);
}

void GameScene::handleMouseRelease(int x, int y, Qt::MouseButtons buttons) {
  emit mouseReleased(x, y ,buttons);
}

void GameScene::handleWheelMove(int x, int y, int delta) {
  emit mouseWheelMoved(x, y, delta);
}

void GameScene::handleHooverEnter(int x, int y) {
  emit hooverEntered(x, y);
}

void GameScene::handleHooverLeave(int x, int y) {
  emit hooverLeft(x, y);
}
