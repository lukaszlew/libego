#ifndef HEXFIELD_H
#define HEXFIELD_H

#include "Field.h"

class HexField: public Field
{

public:
  HexField(int x, int y, QGraphicsItem *parent = 0);
  ~HexField();

private:

};

#endif // HEXFIELD_H
