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
  for (int y = minimalYCoordinate(); y <= maximalYCoordinate(); y++) {
    bool started = false;
    int startX;
    for (int x = minimalXCoordinate(y); x <= maximalXCoordinate(y); x++) {
      if (!started && isValidCoordinate(x, y)) {
        started = true;
        startX = x;
      }
      if (started && !isValidCoordinate(x + 1, y)) {
        started = false;
        lines.push_back(QLineF(getFieldPosition(startX, y), getFieldPosition(x, y)));
      }
    }
  }
  painter->drawLines(lines);
}

void Grid::drawVerticalLines(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) const {
  QVector<QLineF> lines;
  for (int x = minimalXCoordinate(); x <= maximalXCoordinate(); x++) {
    bool started = false;
    int startY;
    for (int y = minimalYCoordinate(x); y <= maximalYCoordinate(x); y++) {
      if (!started && isValidCoordinate(x, y)) {
        started = true;
        startY = y;
      }
      if (started && !isValidCoordinate(x, y + 1)) {
        started = false;
        lines.push_back(QLineF(getFieldPosition(x, startY), getFieldPosition(x, y)));
      }
    }
  }
  painter->drawLines(lines);
}

void Grid::drawRightDiagonalLines(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) const {
  QVector<QLineF> lines;
  for (int sum = minimalXCoordinate() + minimalYCoordinate(); sum < maximalXCoordinate() + maximalYCoordinate(); sum++) {
    bool started = false;
    int startX;
    for (int x = minimalXCoordinate(); x <= maximalXCoordinate(); x++) {
      if (!started && isValidCoordinate(x, sum -x)) {
        started = true;
        startX = x;
      }
      if (started && !isValidCoordinate(x + 1, sum - x - 1)) {
        started = false;
        QPointF startPoint = getFieldPosition(startX, sum - startX);
        QPointF endPoint = getFieldPosition(x, sum - x);
        lines.push_back(QLineF(startPoint, endPoint));
      }
    }
  }
  painter->drawLines(lines);
}

void Grid::drawLeftDiagonalLines(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) const {
  QVector<QLineF> lines;
  for (int diff = minimalYCoordinate() - maximalXCoordinate();
           diff <= maximalYCoordinate() - minimalXCoordinate(); diff++) {
    bool started = false;
    int startX;
    for (int x = minimalXCoordinate(); x <= maximalXCoordinate(); x++) {
      if (!started && isValidCoordinate(x, x + diff)) {
        started = true;
        startX = x;
      }
      if (started && !isValidCoordinate(x + 1, x + 1 + diff)) {
        started = false;
        QPointF startPoint = getFieldPosition(startX, diff + startX);
        QPointF endPoint = getFieldPosition(x, x + diff);
        lines.push_back(QLineF(startPoint, endPoint));
      }
    }
  }
  painter->drawLines(lines);
}

void Grid::drawHandicapSpots(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) const {
  painter->setBrush(Qt::black);
  QList<QPair<int,int> > points = getHandicapCoordinates();

  QPair<int, int> point;
  foreach(point, points)
    {
      if (isValidCoordinate(point.first, point.second))
      painter->drawEllipse(getFieldPosition(point.first, point.second),
          s_handicapSpotRadius , s_handicapSpotRadius);
    }
}

void Grid::drawFieldsShape(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) const {
  QTransform transform = painter->transform();
  for (int x = minimalXCoordinate(); x <= maximalXCoordinate(); x++) {
    for (int y = minimalYCoordinate(x); y <= maximalYCoordinate(x); y++) {
      if (isValidCoordinate(x, y)) {
        painter->setTransform(transform);
        painter->translate(getFieldPosition(x, y));
        painter->drawPath(getPath());
      }
    }
  }
}

Field* Grid::createField(QGraphicsItem * parent) {
  return new Field(getPath(), parent);
}

bool Grid::isValidCoordinate(int x, int y) const {
  return (x >= minimalXCoordinate(y)) && (x <= maximalXCoordinate(y)) &&
    (y >= minimalYCoordinate(x)) && (y <= maximalYCoordinate(x));
}
