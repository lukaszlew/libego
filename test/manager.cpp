#include <QDebug>
#include <boost/bind.hpp>
#include "manager.h"
#include "GameScene.h"

manager::manager(GameScene *scene, QObject * parent): QObject(parent), m_index(0), m_gameScene(scene) {
  m_gameScene->setLeftButtonHandler(boost::bind(&manager::handleLeftClick, this, _1, _2));
  m_gameScene->setRightButtonHandler(boost::bind(&manager::handleRightClick, this, _1, _2));

}

void manager::handleLeftClick(int x, int y) {
  switch (m_index) {
  case 0:
    m_gameScene->addBlackStone(x, y);
    break;
  case 1:
    m_gameScene->addWhiteStone(x, y);
    break;
  case 2:
    m_gameScene->addMark(x, y);
    break;
  case 3:
    m_gameScene->addCircle(x, y);
    break;
  case 4:
    m_gameScene->addSquare(x, y);
    break;
  case 5:
    m_gameScene->addTriangle(x, y);
    break;
  case 6:
    m_gameScene->addLabel(x, y, m_label);
    break;
  default:
    qDebug() << "wrong combobox current index";
    break;
  }

}

void manager::handleRightClick(int x, int y) {
  switch (m_index) {
  case 0:
  case 1:
    m_gameScene->removeStone(x, y);
    break;
  case 2:
    m_gameScene->removeMark(x, y);
    break;
  case 3:
    m_gameScene->removeCircle(x, y);
    break;
  case 4:
    m_gameScene->removeSquare(x, y);
    break;
  case 5:
    m_gameScene->removeTriangle(x, y);
    break;
  case 6:
    m_gameScene->removeLabel(x, y);
    break;
  default:
    qDebug() << "wrong combobox current index";
    break;
  }

}
