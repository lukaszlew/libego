#include "SquareGrid.h"
#include "Ruler.h"

const qreal SquareGrid::s_width = 40;
const qreal SquareGrid::s_height = 40;

QRectF SquareGrid::boundingRect() const {
  QPointF topLeft = getFieldPosition(minimalCoordinate(), minimalCoordinate()) - QPointF(fieldWidth(),
      fieldHeight());
  QPointF bottomRight = getFieldPosition(maximalCoordinate(), maximalCoordinate()) + QPointF(fieldWidth(),
      fieldHeight());
  return QRectF(topLeft, bottomRight);
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
  return QPointF(x * s_width, y * s_height);
}

QPainterPath SquareGrid::getPath() const {
  QPainterPath path;
  path.addRect(-s_width / 2, -s_height / 2, s_width, s_height);
  return path;
}

Ruler* SquareGrid::createRuler() {
  return new Ruler(Ruler::LocateBefore | Ruler::LocateAfter | Ruler::TypeLetters, Ruler::LocateAfter
      | Ruler::LocateBefore, this);
}
