#ifndef GRID_H_
#define GRID_H_

#include <QGraphicsItem>

class Grid: public QGraphicsItem
{
public:
  Grid(int size, QGraphicsItem * parent = 0);
  virtual ~Grid();

protected:
  int m_size;
};

#endif /* GRID_H_ */
