#include <QPainter>

#include "Ruler.h"

#include "Grid.h"

const qreal Ruler::s_size = 15;
const int Ruler::s_fontSize = 20;

Ruler::Ruler(int verticalType, int horizontalType, Grid* grid) :
  QGraphicsItem(grid), m_grid(grid), m_verticalType(verticalType), m_horizontalType(horizontalType) {
}

QRectF Ruler::boundingRect() const {
  QPointF topLeft = m_grid->boundingRect().topLeft();
  QPointF bottomRight = m_grid->boundingRect().bottomRight();
  if (m_horizontalType & LocateBefore)
    topLeft -= QPointF(m_grid->fieldWidth(), 0);
  if (m_horizontalType & LocateAfter)
    bottomRight += QPointF(m_grid->fieldWidth(), 0);
  if (m_verticalType & LocateBefore)
    topLeft -= QPointF(0, m_grid->fieldHeight());
  if (m_verticalType & LocateAfter)
    bottomRight += QPointF(0, m_grid->fieldHeight());
  return QRectF(topLeft, bottomRight);
}

void Ruler::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  QFont font("Arial", s_fontSize);
  painter->setFont(font);

  if (m_horizontalType & LocateBefore)
    for (int y = m_grid->minimalCoordinate(); y <= m_grid->maximalCoordinate(); y++) {
      QRectF rect(m_grid->getFieldPosition(m_grid->minimalXCoordinate(y) - 1, y) - QPointF(s_size, s_size),
          QSizeF(2* s_size , 2* s_size ));
      painter->drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, getCoordinateString(m_horizontalType, y));
    }

  if (m_horizontalType & LocateAfter)
    for (int y = m_grid->minimalCoordinate(); y <= m_grid->maximalCoordinate(); y++) {
      QRectF rect(m_grid->getFieldPosition(m_grid->maximalXCoordinate(y) + 1, y) - QPointF(s_size, s_size),
          QSizeF(2* s_size , 2* s_size ));
      painter->drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, getCoordinateString(m_horizontalType, y));
    }

  if (m_verticalType & LocateBefore)
    for (int x = m_grid->minimalCoordinate(); x <= m_grid->maximalCoordinate(); x++) {
      QRectF rect(m_grid->getFieldPosition(x, m_grid->minimalYCoordinate(x) - 1) - QPointF(s_size, s_size),
          QSizeF(2* s_size , 2* s_size ));
      painter->drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, getCoordinateString(m_verticalType, x));
    }

  if (m_verticalType & LocateAfter)
    for (int x = m_grid->minimalCoordinate(); x <= m_grid->maximalCoordinate(); x++) {
      QRectF rect(m_grid->getFieldPosition(x, m_grid->maximalYCoordinate(x) + 1) - QPointF(s_size, s_size),
          QSizeF(2* s_size , 2* s_size ));
      painter->drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, getCoordinateString(m_verticalType, x));
    }
}

QString Ruler::getCoordinateString(int type, int coord) {
  if (type & TypeLetters) {
    return QString('A' - 1 + coord);
  } else {
    return QString::number(coord);
  }

}
