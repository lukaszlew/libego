#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QWheelEvent>

#include "Board.h"

#include "SquareField.h"

Board::Board(int size, QWidget *parent) :
  QGraphicsView(parent), m_fields(size * size)
{
  QGraphicsScene *scene = new QGraphicsScene(this);
  scene->setBackgroundBrush(QPixmap(":/images/wood.png"));
  setScene(scene);
  setResizeAnchor(QGraphicsView::AnchorViewCenter);
}

Board::~Board()
{

}

void Board::wheelEvent(QWheelEvent *event)
{

  if (event->delta() > 0)
    scale(1.2, 1.2);
  else
    scale(1 / 1.2, 1 / 1.2);

}

void Board::insertBlackStone(const QString& pos)
{
  // znalezienie odpowiedniego pola w m_fields i wywolanie na nim metody insertStone

}
void Board::insertWhiteStone(const QString& pos)
{

}

void Board::removeStone(const QString& pos)
{

}

void Board::markField(const QString& pos, char c)
{

}

void Board::insertCircle(const QString& pos, QColor color)
{

}

void Board::removeCircle(const QString& pos)
{

}

void Board::insertTriangle(const QString& pos, QColor color)
{

}

void Board::removeTriangle(const QString& pos)
{

}

void Board::drawGrid()
{

}
