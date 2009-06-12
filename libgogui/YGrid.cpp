#include "YGrid.h"

#include "Field.h"
#include "Ruler.h"


YGrid::YGrid(int size, QGraphicsItem * parent) : HexGrid(size, parent) {
  QPointF top = getFieldPosition(maximalCoordinate(), minimalYCoordinate(maximalCoordinate()));
  QPointF bottomLeft = getFieldPosition(minimalXCoordinate(maximalCoordinate()), maximalCoordinate());
  QPointF bottomRight = getFieldPosition(maximalXCoordinate(maximalCoordinate()), maximalCoordinate());
  m_rect = QRectF(bottomLeft.x(), top.y(), bottomRight.x() - bottomLeft.x(), bottomLeft.y() - top.y());
}

QList<QPair<int,int> > YGrid::getHandicapCoordinates() const {
  return QList<QPair<int,int> >();
}

void YGrid::paint(QPainter *painter, const QStyleOptionGraphicsItem *style, QWidget *widget) {
  drawFieldsShape(painter, style, widget);
}
