#include <QDebug>
#include <boost/bind.hpp>
#include "manager.h"
#include "BoardScene.h"

manager::manager(BoardScene *scene, QObject * parent): QObject(parent), m_index(0), m_boardScene(scene) {
  m_boardScene->setLeftButtonHandler(boost::bind(&manager::handleLeftClick, this, _1, _2));
  m_boardScene->setRightButtonHandler(boost::bind(&manager::handleRightClick, this, _1, _2));

}

void manager::handleLeftClick(int x, int y) {
  switch (m_index) {
  case 0:
    m_boardScene->addBlackStone(x, y);
    break;
  case 1:
    m_boardScene->addWhiteStone(x, y);
    break;
  case 2:
    m_boardScene->addMark(x, y);
    break;
  case 3:
    m_boardScene->addCircle(x, y);
    break;
  case 4:
    m_boardScene->addSquare(x, y);
    break;
  case 5:
    m_boardScene->addTriangle(x, y);
    break;
  case 6:
    m_boardScene->addLabel(x, y, m_label);
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
    m_boardScene->removeStone(x, y);
    break;
  case 2:
    m_boardScene->removeMark(x, y);
    break;
  case 3:
    m_boardScene->removeCircle(x, y);
    break;
  case 4:
    m_boardScene->removeSquare(x, y);
    break;
  case 5:
    m_boardScene->removeTriangle(x, y);
    break;
  case 6:
    m_boardScene->removeLabel(x, y);
    break;
  default:
    qDebug() << "wrong combobox current index";
    break;
  }

}
