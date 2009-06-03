#include <QPainter>
#include "SquareGrid.h"
#include "SquareField.h"

SquareGrid::SquareGrid(int size, QGraphicsItem * parent) :
  Grid(size, parent)
{
  setPos(SquareField::s_width, SquareField::s_height);
}

SquareGrid::~SquareGrid()
{

}

QRectF SquareGrid::boundingRect() const
{
  int width = (m_size - 1) * SquareField::s_width;
  int height = (m_size - 1) * SquareField::s_height;
  return QRectF(0, 0, width, height);
}

void SquareGrid::paint(QPainter *painter, const QStyleOptionGraphicsItem*,
    QWidget*)
{
  int max_width = (m_size - 1) * SquareField::s_width;
  int max_height = (m_size - 1) * SquareField::s_height;
  QVector<QLine> lines;
  for (int height = 0; height <= max_height; height += SquareField::s_height)
    lines.push_back(QLine(QPoint(0, height), QPoint(max_width, height)));

  for (int width = 0; width <= max_width; width += SquareField::s_width)
    lines.push_back(QLine(QPoint(width, 0), QPoint(width, max_height)));

  painter->drawLines(lines);

}
