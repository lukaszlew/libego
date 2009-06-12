#include "HavannahGrid.h"

#include "Ruler.h"

const qreal HavannahGrid::s_width = 40;
const qreal HavannahGrid::s_height = 40 * 1.73205;

QRectF HavannahGrid::boundingRect() const {
  QPointF top = getFieldPosition(minimalXCoordinate(minimalCoordinate()), minimalCoordinate());
  QPointF left = getFieldPosition(minimalCoordinate(), maximalYCoordinate(minimalCoordinate()));
  QPointF right = getFieldPosition(maximalCoordinate(), minimalYCoordinate(maximalCoordinate()));
  QPointF bottom = getFieldPosition(maximalXCoordinate(maximalCoordinate()), maximalCoordinate());
  return QRectF(QPointF(left.x() - fieldWidth() / 2, top.y() - fieldHeight() / 2),
      QPointF(right.x() + fieldWidth() / 2, bottom.y() + fieldHeight() / 2));
}

void HavannahGrid::paint(QPainter *painter, const QStyleOptionGraphicsItem* style, QWidget* widget) {
  drawFieldsShape(painter, style, widget);
}

QList<QPair<int,int> > HavannahGrid::getHandicapCoordinates() const {
  return QList<QPair<int,int> >();
}

int HavannahGrid::minimalXCoordinate(int y) const {
  int res = m_size / 2 + 2 - y;
  return (res < 1) ? 1 : res;
}

int HavannahGrid::maximalXCoordinate(int y) const {
  int res = m_size + m_size / 2 + 1 - y;
  return (res > m_size) ? m_size : res;
}

int HavannahGrid::minimalYCoordinate(int x) const {
  return minimalXCoordinate(x);
}

int HavannahGrid::maximalYCoordinate(int x) const {
  return maximalXCoordinate(x);
}

QPainterPath HavannahGrid::getPath() const {
  QPolygonF szesciokat(7);
  szesciokat[0] = QPointF(-s_width, 0);
  szesciokat[1] = QPointF(-s_width / 2, -s_height / 2);
  szesciokat[2] = QPointF(s_width / 2, -s_height / 2);
  szesciokat[3] = QPointF(s_width, 0);
  szesciokat[4] = QPointF(s_width / 2, s_height / 2);
  szesciokat[5] = QPointF(-s_width / 2, s_height / 2);
  szesciokat[6] = QPointF(-s_width, 0);
  QPainterPath path;
  path.addPolygon(szesciokat);
  return path;
}

Ruler* HavannahGrid::createRuler() {
  return new Ruler(Ruler::LocateAfter | Ruler::TypeLetters, Ruler::LocateBefore, this);
}

QPointF HavannahGrid::getFieldPosition(int x, int y) const {
  return QPointF(1.5 * x * s_width, (0.5 * x + y) * s_height);
}
