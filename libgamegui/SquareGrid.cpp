#include "SquareGrid.h"
#include "Ruler.h"

const qreal SquareGrid::s_width = 40;
const qreal SquareGrid::s_height = 40;

QRectF SquareGrid::boundingRect() const {
  QPointF topLeft = getFieldPosition(minimalXCoordinate(), minimalYCoordinate()) - fieldSize() / 2;
  QPointF bottomRight = getFieldPosition(maximalXCoordinate(), maximalYCoordinate()) + fieldSize() / 2;
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
    int diff = (m_size < 13) ? 2 : 3;

    res.push_back(QPair<int, int> (minimalXCoordinate() + diff, minimalYCoordinate() + diff));
    res.push_back(QPair<int, int> (minimalXCoordinate() + diff, maximalYCoordinate() - diff));
    res.push_back(QPair<int, int> (maximalXCoordinate() - diff, minimalYCoordinate() + diff));
    res.push_back(QPair<int, int> (maximalXCoordinate() - diff, maximalYCoordinate() - diff));

    if (m_size & 1 && m_size > 8) {
      int middleXField = (minimalXCoordinate() + maximalXCoordinate()) / 2;
      int middleYField = (minimalYCoordinate() + maximalYCoordinate()) / 2;
      res.push_back(QPair<int, int> (middleXField, minimalYCoordinate() + diff));
      res.push_back(QPair<int, int> (middleXField, maximalYCoordinate() - diff));
      res.push_back(QPair<int, int> (minimalXCoordinate() + diff, middleYField));
      res.push_back(QPair<int, int> (maximalXCoordinate() - diff, middleYField));
      res.push_back(QPair<int, int> (middleXField, middleYField));
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
