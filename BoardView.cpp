#include <QWheelEvent>

#include "BoardView.h"

BoardView::BoardView(QWidget *parent) :
  QGraphicsView(parent)
{
  setResizeAnchor(QGraphicsView::AnchorViewCenter);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

BoardView::BoardView(QGraphicsScene * scene, QWidget * parent) :
  QGraphicsView(scene, parent)
{
  setResizeAnchor(QGraphicsView::AnchorViewCenter);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}


BoardView::~BoardView()
{
}

void BoardView::wheelEvent(QWheelEvent *event)
{
  if (event->delta() > 0)
    scale(1.2, 1.2);
  else
    scale(1 / 1.2, 1 / 1.2);
}

void BoardView::resizeEvent(QResizeEvent* )
{
  /* whole scene should be visible */
  fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}

