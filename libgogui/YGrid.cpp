#include "YGrid.h"

YGrid::YGrid(int size, QGraphicsItem * parent) : HexGrid(size, parent) {}

QRectF YGrid::boundingRect() const {
  QPointF top = getFieldPosition(maximalCoordinate(), minimalYCoordinate(maximalCoordinate()));
  QPointF bottomLeft = getFieldPosition(minimalXCoordinate(maximalCoordinate()), maximalCoordinate());
  QPointF bottomRight = getFieldPosition(maximalXCoordinate(maximalCoordinate()), maximalCoordinate());
  return QRectF(bottomLeft.x(), top.y(), bottomRight.x() - bottomLeft.x(), bottomLeft.y() - top.y());
}

void YGrid::paint(QPainter *painter, const QStyleOptionGraphicsItem *style, QWidget *widget) {
  drawFieldsShape(painter, style, widget);
}
