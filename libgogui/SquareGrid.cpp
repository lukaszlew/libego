#include "SquareGrid.h"
#include "SquareField.h"

QRectF SquareGrid::boundingRect() const {
  return QRectF(getFieldPosition(minimalCoordinate(), minimalCoordinate()),
      getFieldPosition(maximalCoordinate(), maximalCoordinate()));
}

void SquareGrid::paint(QPainter *painter, const QStyleOptionGraphicsItem* style, QWidget* widget) {
  drawVerticalLines(painter, style, widget);
  drawHorizontalLines(painter, style, widget);
  drawHandicapSpots(painter, style, widget);
}

QList<QPair<int, int> > SquareGrid::getHandicapCoordinates() const {
  QList<QPair<int, int> > res;

  if (m_size > 4) {
    int minField = (m_size < 13) ? 3 : 4;
    int maxField = maximalCoordinate() - minField + 1;

    res.push_back(QPair<int, int> (minField, minField));
    res.push_back(QPair<int, int> (minField, maxField));
    res.push_back(QPair<int, int> (maxField, minField));
    res.push_back(QPair<int, int> (maxField, maxField));

    if (m_size & 1 && m_size > 8) {
      int middleField = (minimalCoordinate() + maximalCoordinate()) / 2;
      res.push_back(QPair<int, int> (middleField, minField));
      res.push_back(QPair<int, int> (middleField, maxField));
      res.push_back(QPair<int, int> (minField, middleField));
      res.push_back(QPair<int, int> (maxField, middleField));
      res.push_back(QPair<int, int> (middleField, middleField));
    }
  }
  return res;
}

QPointF SquareGrid::getFieldPosition(int x, int y) const {
  return QPointF(x * SquareField::s_width, y * SquareField::s_height);
}

Field* SquareGrid::createField(int x, int y, QGraphicsItem *parent) const {
  return new SquareField(x, y, parent);
}
