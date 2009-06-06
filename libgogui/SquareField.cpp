#include <QPen>
#include <QBrush>

#include "SquareField.h"

const int SquareField::s_width = 40;
const int SquareField::s_height = 40;

SquareField::SquareField(int x, int y, QGraphicsItem *parent) :
  Field(x, y, parent)
{
  setPos(x * s_width, y * s_height);
  initializeBackgroudItem();
}

SquareField::~SquareField()
{

}

void SquareField::initializeBackgroudItem()
{
  m_background = new QGraphicsRectItem(-s_width / 2, -s_height / 2, s_width,
      s_height);
  QPen pen;
  pen.setStyle(Qt::NoPen);
  m_background->setPen(pen);
  m_background->setPos(pos());
  QBrush brush;
  brush.setColor(Qt::blue);
  m_background->setBrush(brush);

  addToGroup(m_background);
}
