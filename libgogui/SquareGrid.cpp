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

  //handicap spots
  if (m_size > 6) {
    painter->setBrush(Qt::black);

    int heightFromBorder = SquareField::s_height;
    int widthFromBorder = SquareField::s_width;
    if (m_size < 13) {
      heightFromBorder *= 2;
      widthFromBorder *= 2;
    } else {
      heightFromBorder *= 3;
      widthFromBorder *= 3;
    }

    qreal r = 2;

    painter->drawEllipse(widthFromBorder - r, heightFromBorder - r, 2*r, 2*r);
    painter->drawEllipse(max_width - widthFromBorder - r, heightFromBorder - r, 2*r, 2*r);
    painter->drawEllipse(widthFromBorder - r, max_height - heightFromBorder - r, 2*r, 2*r);
    painter->drawEllipse(max_width - widthFromBorder - r, max_height - heightFromBorder - r, 2*r, 2*r);

    if (m_size & 1 && m_size > 8) {
      painter->drawEllipse(widthFromBorder - r, max_height / 2 - r, 2*r, 2*r);
      painter->drawEllipse(max_width / 2 - r, heightFromBorder - r, 2*r, 2*r);
      painter->drawEllipse(max_width / 2 - r, max_height / 2 - r, 2*r, 2*r);
      painter->drawEllipse(max_width / 2 - r, max_height - heightFromBorder - r, 2*r, 2*r);
      painter->drawEllipse(max_width - widthFromBorder - r, max_height / 2 - r, 2*r, 2*r);
    }
  }
}
