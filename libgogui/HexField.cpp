#include "HexField.h"

const qreal HexField::s_width = 40 * 1.73205;
const qreal HexField::s_height = 40;

HexField::HexField(int x, int y, QGraphicsItem *parent) :
  Field(x, y, parent) {
  QPolygonF szesciokat(6);
  szesciokat[0] = QPointF(0, -s_height);
  szesciokat[1] = QPointF(s_width / 2, -s_height / 2);
  szesciokat[2] = QPointF(s_width / 2, s_height / 2);
  szesciokat[3] = QPointF(0, s_height);
  szesciokat[4] = QPointF(-s_width / 2, s_height / 2);
  szesciokat[5] = QPointF(-s_width / 2, -s_height / 2);
  m_background = new QGraphicsPolygonItem(szesciokat);
  QPen pen;
  pen.setStyle(Qt::NoPen);
  m_background->setPen(pen);
  m_background->setZValue(s_backgroundZValue);
  m_background->setPos(pos());
  addToGroup(m_background);
}
