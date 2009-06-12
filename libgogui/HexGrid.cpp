#include "HexGrid.h"

#include "Ruler.h"

const qreal HexGrid::s_width = 40 * 1.73205;
const qreal HexGrid::s_height = 40;

HexGrid::HexGrid(int size, QGraphicsItem * parent) :
  Grid(size, parent) {
  QPointF LL = getFieldPosition(minimalCoordinate(), minimalYCoordinate(minimalCoordinate()));
  QPointF LH = getFieldPosition(minimalCoordinate(), maximalYCoordinate(minimalCoordinate()));
  QPointF HL = getFieldPosition(maximalCoordinate(), minimalYCoordinate(maximalCoordinate()));
  QPointF HH = getFieldPosition(maximalCoordinate(), maximalYCoordinate(maximalCoordinate()));
  m_rect = QRectF(LL.x(), HL.y(), HH.x() - LL.x(), LH.y() - HL.y());
}

void HexGrid::paint(QPainter *painter, const QStyleOptionGraphicsItem* style, QWidget* widget) {
  drawVerticalLines(painter, style, widget);
  drawHorizontalLines(painter, style, widget);
  drawDiagonalLines(painter, style, widget);
  drawHandicapSpots(painter, style, widget);
}


QList<QPair<int, int> > HexGrid::getHandicapCoordinates() const {
  QList<QPair<int, int> > res;

  if (m_size > 4) {
    int diff = (m_size < 13) ? 2 : 3;
    int minField = minimalCoordinate() + diff;
    int maxField = maximalCoordinate() - diff;

    res.push_back(QPair<int, int> (minField, minimalYCoordinate(minField) + diff));
    res.push_back(QPair<int, int> (minField, maximalYCoordinate(minField) - diff));
    res.push_back(QPair<int, int> (maxField, minimalYCoordinate(maxField) + diff));
    res.push_back(QPair<int, int> (maxField, maximalYCoordinate(maxField) - diff));
    res.push_back(QPair<int, int> (minimalXCoordinate(minField) + diff, minField));
    res.push_back(QPair<int, int> (maximalXCoordinate(maxField) - diff, maxField));
    res.push_back(QPair<int, int> (minimalXCoordinate(minField) + diff, maximalYCoordinate(maxField) - diff));
  }
  return res;
}

int HexGrid::minimalXCoordinate(int y) const {
  int res = m_size / 2 + 2 - y;
  return (res < 1) ? 1 : res;
}

int HexGrid::maximalXCoordinate(int y) const {
  int res = m_size + m_size / 2 + 1 - y;
  return (res > m_size) ? m_size : res;
}

int HexGrid::minimalYCoordinate(int x) const {
  return minimalXCoordinate(x);
}

int HexGrid::maximalYCoordinate(int x) const {
  return maximalXCoordinate(x);
}

QPainterPath HexGrid::getPath() const {
  QPolygonF szesciokat(7);
  szesciokat[0] = QPointF(0, -s_height);
  szesciokat[1] = QPointF(s_width / 2, -s_height / 2);
  szesciokat[2] = QPointF(s_width / 2, s_height / 2);
  szesciokat[3] = QPointF(0, s_height);
  szesciokat[4] = QPointF(-s_width / 2, s_height / 2);
  szesciokat[5] = QPointF(-s_width / 2, -s_height / 2);
  szesciokat[6] = QPointF(0, -s_height);
  QPainterPath path;
  path.addPolygon(szesciokat);
  return path;
}

Ruler* HexGrid::createRuler() {
  return new Ruler(Ruler::LocateAfter, Ruler::LocateBefore | Ruler::TypeLetters, this);
}

QPointF HexGrid::getFieldPosition(int x, int y) const {
  return QPointF((x + 0.5 * y) * s_width, y * 1.5 * s_height);
}
