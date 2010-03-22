#ifndef BOARDVIEW_H_
#define BOARDVIEW_H_

#include <QGraphicsView>

class ResizableView: public QGraphicsView
{
Q_OBJECT

public:
  ResizableView(QWidget *parent = 0);
  ResizableView(QGraphicsScene * scene, QWidget * parent = 0);
  ~ResizableView();

protected:
  void wheelEvent(QWheelEvent *event);
  virtual void resizeEvent(QResizeEvent * event);
};
#endif /* BOARDVIEW_H_ */
