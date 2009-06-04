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
  Q_UNUSED(pos);
  // TODO
  // znalezienie odpowiedniego pola w m_fields i wywolanie na nim metody insertStone
  return NULL;
}

QGraphicsItem* BoardScene::addWhiteStone(const QString& pos)
{
  Q_UNUSED(pos);
  //TODO
  return NULL;

}

void BoardScene::removeStone(const QString& pos)
{
  Q_UNUSED(pos);
  //TODO
}

QGraphicsItem* BoardScene::addLabel(const QString& pos, const QString& label)
{
  Q_UNUSED(pos);
  Q_UNUSED(label);
  //TODO
  return NULL;
}

void BoardScene::::removeLabel(const QString& pos)
{
  //TODO
}

QGraphicsItem* BoardScene::addCircle(const QString& pos, QColor color)
{
  Q_UNUSED(pos);
  Q_UNUSED(color);
  //TODO
  return NULL;
}

void BoardScene::removeCircle(const QString& pos)
{
  Q_UNUSED(pos);
  //TODO
}

QGraphicsItem* BoardScene::addTriangle(const QString& pos, QColor color)
{
  Q_UNUSED(pos);
  Q_UNUSED(color);
  //TODO
  return NULL;
}

void BoardScene::removeTriangle(const QString& pos)
{
  Q_UNUSED(pos);
  //TODO
}

void BoardScene::removeRuler()
{
  //TODO
}

QString BoardScene::getFieldString(int x, int y)
{
  QString res;
  res += QString::number(x);
  res += '-';
  res += QString::number(y);
  return res;
}
