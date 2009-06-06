#ifndef SQUAREFIELD_H
#define SQUAREFIELD_H

#include "Field.h"

class SquareField: public Field
{

public:
  SquareField(int x, int y, QGraphicsItem *parent = 0);
  ~SquareField();

  virtual void initializeBackgroudItem();
  virtual int getWidth() const
  {
    return s_width;
  }
  virtual int getHeight() const
  {
    return s_height;
  }

  static const int s_width, s_height;
protected:

};

#endif // SQUAREFIELD_H
