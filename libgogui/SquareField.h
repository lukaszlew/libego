#ifndef SQUAREFIELD_H
#define SQUAREFIELD_H

#include "Field.h"

class SquareField: public Field
{

public:
  SquareField(int x, int y, QGraphicsItem *parent = 0);

  virtual void initializeBackgroudItem();
  virtual qreal getWidth() const
  {
    return s_width;
  }
  virtual qreal getHeight() const
  {
    return s_height;
  }

  static const qreal s_width, s_height;
};

#endif // SQUAREFIELD_H
