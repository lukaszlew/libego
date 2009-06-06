#ifndef BOARDVIEW_H_
#define BOARDVIEW_H_

#include <QGraphicsView>

class BoardView: public QGraphicsView
{
Q_OBJECT

public:
  BoardView(QWidget *parent = 0);
  BoardView(QGraphicsScene * scene, QWidget * parent = 0);
  ~BoardView();

protected:
  void wheelEvent(QWheelEvent *event);
  virtual void resizeEvent(QResizeEvent * event);
};
#endif /* BOARDVIEW_H_ */
