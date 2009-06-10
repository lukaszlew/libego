#include <QDebug>
#include <QPainter>
#include "Grid.h"

const qreal Grid::s_gridZVlaue = 0.0;
const qreal Grid::s_handicapSpotRadius = 3.0;

Grid::Grid(int size, QGraphicsItem * parent) :
  QGraphicsItem(parent), m_size(size)
{
  setZValue(s_gridZVlaue);
}

void Grid::drawHorizontalLines(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
  QVector<QLineF> lines;
  for (int x = minimalCoordinate(); x <= maximalCoordinate(); x++) {
    lines.push_back(QLineF(getFieldPosition(minimalCoordinate(x), x), getFieldPosition(maximalCoordinate(x),
        x)));
  }
  painter->drawLines(lines);
}

void Grid::drawVerticalLines(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
  QVector<QLineF> lines;
  for (int x = minimalCoordinate(); x <= maximalCoordinate(); x++) {
    QPointF startPoint = getFieldPosition(x, minimalCoordinate(x));
    QPointF endPoint = getFieldPosition(x, maximalCoordinate(x));
    lines.push_back(QLineF(startPoint, endPoint));
  }
  painter->drawLines(lines);
}

void Grid::drawDiagonalLines(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
  QVector<QLineF> lines;
  for (int x = minimalCoordinate(); x < maximalCoordinate(); x++) {
    if (x >= minimalCoordinate(minimalCoordinate())) {
      QPointF startPoint = getFieldPosition(minimalCoordinate(), x);
      QPointF endPoint = getFieldPosition(x, minimalCoordinate());
      lines.push_back(QLineF(startPoint, endPoint));
    }
  }
  for (int x = minimalCoordinate(); x < maximalCoordinate(); x++) {
    if (x <= maximalCoordinate(maximalCoordinate())) {
      QPointF startPoint = getFieldPosition(maximalCoordinate(), x);
      QPointF endPoint = getFieldPosition(x, maximalCoordinate());
      lines.push_back(QLineF(startPoint, endPoint));
    }
  }
  painter->drawLines(lines);

}

void Grid::drawHandicapSpots(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
  painter->setBrush(Qt::black);
  QList<QPair<int,int> > points = getHandicapCoordinates();

  typedef QPair<int,int> intpair;
  foreach(intpair point, points)
    {
      painter->drawEllipse(getFieldPosition(point.first, point.second),
          s_handicapSpotRadius , s_handicapSpotRadius);
    }
}

bool Grid::isValidCoordinate(int x, int y) const {
  return (y >= minimalCoordinate(x)) && (y <= maximalCoordinate(x));
}
