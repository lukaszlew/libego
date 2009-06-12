#include "HoGrid.h"

void HoGrid::paint(QPainter *painter, const QStyleOptionGraphicsItem* style, QWidget* widget) {
  drawVerticalLines(painter, style, widget);
  drawHorizontalLines(painter, style, widget);
  drawDiagonalLines(painter, style, widget);
  drawHandicapSpots(painter, style, widget);
}

QList<QPair<int, int> > HoGrid::getHandicapCoordinates() const {
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

int HoGrid::minimalXCoordinate(int y) const {
  int res = m_size / 2 + 2 - y;
  return (res < 1) ? 1 : res;
}

int HoGrid::maximalXCoordinate(int y) const {
  int res = m_size + m_size / 2 + 1 - y;
  return (res > m_size) ? m_size : res;
}

int HoGrid::minimalYCoordinate(int x) const {
  return minimalXCoordinate(x);
}

int HoGrid::maximalYCoordinate(int x) const {
  return maximalXCoordinate(x);
}
