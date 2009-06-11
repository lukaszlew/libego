#include "HexGrid.h"

#include "HexField.h"
#include "Ruler.h"

HexGrid::HexGrid(int size, QGraphicsItem * parent) :
  Grid(size, parent) {
  QPointF LL = getFieldPosition(minimalCoordinate(), minimalCoordinate(minimalCoordinate()));
  QPointF LH = getFieldPosition(minimalCoordinate(), maximalCoordinate(minimalCoordinate()));
  QPointF HL = getFieldPosition(maximalCoordinate(), minimalCoordinate(maximalCoordinate()));
  QPointF HH = getFieldPosition(maximalCoordinate(), maximalCoordinate(maximalCoordinate()));
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

    res.push_back(QPair<int, int> (minField, minimalCoordinate(minField) + diff));
    res.push_back(QPair<int, int> (minField, maximalCoordinate(minField) - diff));
    res.push_back(QPair<int, int> (maxField, minimalCoordinate(maxField) + diff));
    res.push_back(QPair<int, int> (maxField, maximalCoordinate(maxField) - diff));
    res.push_back(QPair<int, int> (minimalCoordinate(minField) + diff, minField));
    res.push_back(QPair<int, int> (maximalCoordinate(maxField) - diff, maxField));
    res.push_back(QPair<int, int> (minimalCoordinate(minField) + diff, maximalCoordinate(maxField) - diff));
  }
  return res;
}

int HexGrid::minimalCoordinate(int x) const {
  int res = m_size / 2 + 2 - x;
  return (res < 1) ? 1 : res;
}

int HexGrid::maximalCoordinate(int x) const {
  int res = m_size + m_size / 2 + 1 - x;
  return (res > m_size) ? m_size : res;
}

QPointF HexGrid::getFieldPosition(int x, int y) const {
  return QPointF((x + 0.5 * y) * HexField::s_width, y * 1.5 * HexField::s_height);
}

Field* HexGrid::createField(int x, int y) {
  return new HexField(x, y, this);
}

Ruler* HexGrid::createRuler() {
  return new Ruler(Ruler::LocateBefore, Ruler::LocateAfter | Ruler::TypeLetters, this);
}


