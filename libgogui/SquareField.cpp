#include <QPen>
#include <QBrush>

#include "SquareField.h"

const qreal SquareField::s_width = 40;
const qreal SquareField::s_height = 40;

SquareField::SquareField(int x, int y, QGraphicsItem *parent) :
  Field(x, y, parent) {
  initializeBackgroudItem();
}

void SquareField::initializeBackgroudItem() {
  m_background = new QGraphicsRectItem(-s_width / 2, -s_height / 2, s_width, s_height);
  QPen pen;
  pen.setStyle(Qt::NoPen);
  m_background->setPen(pen);
  m_background->setPos(pos());
  m_background->setZValue(s_backgroundZValue);
  addToGroup(m_background);
}
