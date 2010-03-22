#include <QWheelEvent>

#include "ResizableView.h"

ResizableView::ResizableView(QWidget *parent) :
  QGraphicsView(parent)
{
  setResizeAnchor(QGraphicsView::AnchorViewCenter);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

ResizableView::ResizableView(QGraphicsScene * scene, QWidget * parent) :
  QGraphicsView(scene, parent)
{
  setResizeAnchor(QGraphicsView::AnchorViewCenter);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}


ResizableView::~ResizableView()
{
}

void ResizableView::wheelEvent(QWheelEvent *event)
{
  Q_UNUSED(event);
  ///*
  if (event->delta() > 0)
    scale(1.2, 1.2);
  else
    scale(1 / 1.2, 1 / 1.2);
  //*/
}

void ResizableView::resizeEvent(QResizeEvent* event)
{
  /* whole scene should be visible */
  fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
  QGraphicsView::resizeEvent(event);
}

