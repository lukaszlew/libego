#include "HexGrid.h"

#include "Ruler.h"

const qreal HexGrid::s_width = 40 * 1.73205;
const qreal HexGrid::s_height = 80;

QRectF HexGrid::boundingRect() const {
  QPointF LL = getFieldPosition(minimalXCoordinate(), minimalYCoordinate(minimalXCoordinate()));
  QPointF LH = getFieldPosition(minimalXCoordinate(), maximalYCoordinate(minimalXCoordinate()));
  QPointF HL = getFieldPosition(maximalXCoordinate(), minimalYCoordinate(maximalXCoordinate()));
  QPointF HH = getFieldPosition(maximalXCoordinate(), maximalYCoordinate(maximalXCoordinate()));
  return QRectF(QPointF(LL.x(), HL.y()) - fieldSize() / 2, QPointF(HH.x(), LH.y()) + fieldSize() / 2);
}

void HexGrid::paint(QPainter *painter, const QStyleOptionGraphicsItem* style, QWidget* widget) {
  drawFieldsShape(painter, style, widget);
}

QList<QPair<int, int> > HexGrid::getHandicapCoordinates() const {
  return QList<QPair<int, int> >();
}

QPainterPath HexGrid::getPath() const {
  QPolygonF szesciokat(7);
  szesciokat[0] = QPointF(0, -s_height / 2);
  szesciokat[1] = QPointF(s_width / 2, -s_height / 4);
  szesciokat[2] = QPointF(s_width / 2, s_height / 4);
  szesciokat[3] = QPointF(0, s_height / 2);
  szesciokat[4] = QPointF(-s_width / 2, s_height / 4);
  szesciokat[5] = QPointF(-s_width / 2, -s_height / 4);
  szesciokat[6] = QPointF(0, -s_height / 2);
  QPainterPath path;
  path.addPolygon(szesciokat);
  return path;
}

Ruler* HexGrid::createRuler() {
  return new Ruler(Ruler::LocateAfter | Ruler::LocateBefore | Ruler::TypeLetters, Ruler::LocateBefore | Ruler::LocateAfter, this);
}

QPointF HexGrid::getFieldPosition(int x, int y) const {
  return QPointF((x + 0.5 * y) * s_width, y * 0.75 * s_height);
}
