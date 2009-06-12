#include "YGrid.h"
#include "Ruler.h"

YGrid::YGrid(int size, QGraphicsItem * parent) : HexGrid(size, parent) {}

QRectF YGrid::boundingRect() const {
  QPointF top = getFieldPosition(maximalCoordinate(), minimalYCoordinate(maximalCoordinate()));
  QPointF bottomLeft = getFieldPosition(minimalXCoordinate(maximalCoordinate()), maximalCoordinate());
  QPointF bottomRight = getFieldPosition(maximalXCoordinate(maximalCoordinate()), maximalCoordinate());
  return QRectF(QPointF(bottomLeft.x(), top.y()) - fieldSize() / 2, bottomRight + fieldSize() / 2);
}

void YGrid::paint(QPainter *painter, const QStyleOptionGraphicsItem *style, QWidget *widget) {
  drawFieldsShape(painter, style, widget);
}

Ruler* YGrid::createRuler() {
  return new Ruler( Ruler::LocateAfter | Ruler::TypeLetters, Ruler::LocateBefore, this);
}
