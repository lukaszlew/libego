#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include "BoardScene.h"
#include "Grid.h"

#include "Field.h"

BoardScene::BoardScene(int size, QObject *parent) :
  QGraphicsScene(parent), m_size(size) {
  setBackgroundBrush(QPixmap(":/images/wood.png"));
  //DEBUG
  connect(this, SIGNAL(fieldClicked(const QString&, Qt::MouseButtons)), this,
      SLOT(debugClick(const QString&, Qt::MouseButtons)));
}

QGraphicsItem* BoardScene::addShape(const QString& pos, EShapeType type) {
  map_type::iterator it = m_fields.find(pos);
  if (it != m_fields.end()) {
    switch (type) {
    case TypeBlackStone:
      return (*it)->addStone(Field::StoneBlack);
    case TypeWhiteStone:
      return (*it)->addStone(Field::StoneWhite);
    case TypeMark:
      return (*it)->addMark();
    case TypeCircle:
      return (*it)->addCircle();
    case TypeSquare:
      return (*it)->addSquare();
    case TypeTriangle:
      return (*it)->addTriangle();
    default:
      return NULL;
    }
  } else {
    return NULL;
  }
}

void BoardScene::removeShape(const QString& pos, EShapeType type) {
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

QGraphicsItem* BoardScene::addBlackStone(const QString& pos) {
  return addShape(pos, TypeBlackStone);
}

QGraphicsItem* BoardScene::addWhiteStone(const QString& pos) {
  return addShape(pos, TypeWhiteStone);
}

void BoardScene::removeStone(const QString& pos) {
  removeShape(pos, TypeBlackStone);
}

QGraphicsItem* BoardScene::addMark(const QString& pos) {
  return addShape(pos, TypeMark);
}

void BoardScene::removeMark(const QString& pos) {
  removeShape(pos, TypeMark);
}

QGraphicsItem* BoardScene::addCircle(const QString& pos) {
  return addShape(pos, TypeCircle);
}

void BoardScene::removeCircle(const QString& pos) {
  removeShape(pos, TypeCircle);
}

QGraphicsItem* BoardScene::addSquare(const QString& pos) {
  return addShape(pos, TypeSquare);
}

void BoardScene::removeSquare(const QString& pos) {
  removeShape(pos, TypeSquare);
}

QGraphicsItem* BoardScene::addTriangle(const QString& pos) {
  return addShape(pos, TypeTriangle);
}

void BoardScene::removeTriangle(const QString& pos) {
  removeShape(pos, TypeTriangle);
}

QGraphicsItem* BoardScene::addLabel(const QString& pos, const QString& label) {
  map_type::iterator it = m_fields.find(pos);
  if (it != m_fields.end()) {
    return (*it)->addLabel(label);
  } else {
    return NULL;
  }
}

void BoardScene::removeLabel(const QString& pos) {
  removeShape(pos, TypeLabel);
}

void BoardScene::removeRuler() {
  if (m_ruler) {
    removeItem(m_ruler);
    delete m_ruler;
    m_ruler = NULL;
  }
}

QString BoardScene::getFieldString(int x, int y) {
  QString res;
  res += QString::number(x);
  res += '-';
  res += QString::number(y);
  return res;
}

///*
void BoardScene::debugClick(const QString& pos, Qt::MouseButtons buttons) {
  QString button;
  if (buttons & Qt::LeftButton) {
    addBlackStone(pos);
    button += "left ";
  }
  if (buttons & Qt::RightButton) {
    removeStone(pos);
    button += "right ";
  }
  if (buttons & Qt::MidButton) {
    addMark(pos);
    button += "mid ";
  }
  if (buttons & Qt::XButton1)
    button += "mid ";
}//*/

///*
void BoardScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent) {
  QList<QGraphicsItem *> itemList = items(mouseEvent->buttonDownScenePos(mouseEvent->button()));
  foreach (QGraphicsItem *item, itemList) {
      Field* field = qgraphicsitem_cast<Field*> (item);

      if (!field) continue;

      if (field->contains(field->mapFromScene(mouseEvent->buttonDownScenePos(mouseEvent->button()))))
        emit fieldClicked(getFieldString(field->x(), field->y()), mouseEvent->buttons());

    }
  QGraphicsScene::mousePressEvent(mouseEvent);
}
//*/
