#ifndef HEXFIELD_H
#define HEXFIELD_H

#include "Field.h"

class HexField: public Field
{
public:
  HexField(int x, int y, QGraphicsItem *parent = 0);

  virtual void initializeBackgroudItem();
  virtual qreal getWidth() const {
    return s_width;
  }
  virtual qreal getHeight() const {
    return s_height;
  }

  static const qreal s_width, s_height;
};

#endif // HEXFIELD_H
