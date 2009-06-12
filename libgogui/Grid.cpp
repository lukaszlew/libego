#include <QPainter>
#include <QPair>

#include "Grid.h"
#include "Field.h"

const qreal Grid::s_gridZVlaue = 0.0;
const qreal Grid::s_handicapSpotRadius = 3.0;

Grid::Grid(int size, QGraphicsItem * parent) :
  QGraphicsItem(parent), m_size(size)
{
  setZValue(s_gridZVlaue);
}

void Grid::drawHorizontalLines(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) const {
  QVector<QLineF> lines;
  for (int y = minimalCoordinate(); y <= maximalCoordinate(); y++) {
    QPointF startPoint = getFieldPosition(minimalXCoordinate(y), y);
    QPointF endPoint = getFieldPosition(maximalXCoordinate(y), y);
    lines.push_back(QLineF(startPoint, endPoint));
  }
  painter->drawLines(lines);
}

void Grid::drawVerticalLines(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) const {
  QVector<QLineF> lines;
  for (int x = minimalCoordinate(); x <= maximalCoordinate(); x++) {
    QPointF startPoint = getFieldPosition(x, minimalYCoordinate(x));
    QPointF endPoint = getFieldPosition(x, maximalYCoordinate(x));
    lines.push_back(QLineF(startPoint, endPoint));
  }
  painter->drawLines(lines);
}

void Grid::drawDiagonalLines(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) const {
  QVector<QLineF> lines;
  for (int x = minimalCoordinate(); x < maximalCoordinate(); x++) {
    if (x >= minimalXCoordinate(minimalCoordinate())) {
      QPointF startPoint = getFieldPosition(minimalCoordinate(), x);
      QPointF endPoint = getFieldPosition(x, minimalCoordinate());
      lines.push_back(QLineF(startPoint, endPoint));
    }
  }
  for (int x = minimalCoordinate(); x < maximalCoordinate(); x++) {
    if (x <= maximalXCoordinate(maximalCoordinate())) {
      QPointF startPoint = getFieldPosition(maximalCoordinate(), x);
      QPointF endPoint = getFieldPosition(x, maximalCoordinate());
      lines.push_back(QLineF(startPoint, endPoint));
    }
  }
  painter->drawLines(lines);
}

void Grid::drawHandicapSpots(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) const {
  painter->setBrush(Qt::black);
  QList<QPair<int,int> > points = getHandicapCoordinates();

  QPair<int, int> point(0,0);
  foreach(point, points)
    {
      painter->drawEllipse(getFieldPosition(point.first, point.second),
          s_handicapSpotRadius , s_handicapSpotRadius);
    }
}

void Grid::drawFieldsShape(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) const {
  QTransform transform = painter->transform();
  for (int x = minimalCoordinate(); x <= maximalCoordinate(); x++) {
    for (int y = minimalYCoordinate(x); y <= maximalYCoordinate(x); y++) {
      painter->setTransform(transform);
      painter->translate(getFieldPosition(x, y));
      painter->drawPath(getPath());
    }
  }
}

Field* Grid::createField(int x, int y, QGraphicsItem * parent) {
  return new Field(getPath(), x, y, parent);
}

bool Grid::isValidCoordinate(int x, int y) const {
  return (x >= minimalXCoordinate(y)) && (x <= maximalXCoordinate(y)) &&
    (y >= minimalYCoordinate(x)) && (y <= maximalYCoordinate(x));
}
