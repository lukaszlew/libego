#include "HoGrid.h"
#include "Ruler.h"

void HoGrid::paint(QPainter *painter, const QStyleOptionGraphicsItem* style, QWidget* widget) {
  drawVerticalLines(painter, style, widget);
  drawHorizontalLines(painter, style, widget);
  drawRightDiagonalLines(painter, style, widget);
  drawHandicapSpots(painter, style, widget);
}

QList<QPair<int, int> > HoGrid::getHandicapCoordinates() const {
  QList<QPair<int, int> > res;

  if (m_size > 4) {
    int diff = (m_size < 13) ? 2 : 3;
    int minXField = minimalXCoordinate() + diff;
    int maxXField = maximalXCoordinate() - diff;
    int minYField = minimalYCoordinate() + diff;
    int maxYField = maximalYCoordinate() - diff;


    res.push_back(QPair<int, int> (minXField, minimalYCoordinate(minXField) + diff));
    res.push_back(QPair<int, int> (minXField, maximalYCoordinate(minXField) - diff));
    res.push_back(QPair<int, int> (maxXField, minimalYCoordinate(maxXField) + diff));
    res.push_back(QPair<int, int> (maxXField, maximalYCoordinate(maxXField) - diff));
    res.push_back(QPair<int, int> (minimalXCoordinate(minYField) + diff, minYField));
    res.push_back(QPair<int, int> (maximalXCoordinate(maxYField) - diff, maxYField));
    res.push_back(QPair<int, int> (minimalXCoordinate(minYField) + diff, maximalYCoordinate(maxXField) - diff));
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

Ruler* HoGrid::createRuler() {
  return new Ruler(Ruler::LocateAfter | Ruler::TypeLetters, Ruler::LocateBefore, this);
}
