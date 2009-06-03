#include "SquareBoard.h"
#include "SquareField.h"
#include "SquareGrid.h"

SquareBoard::SquareBoard(int size, QWidget *parent) :
  Board(size, parent)
{
  m_grid = new SquareGrid(size);
  scene()->addItem(m_grid);

  for (int x = 0; x < size; x++)
    for (int y = 0; y < size; y++) {
      Field *field = new SquareField(x + 1, y + 1);
      m_fields[size * x + y] = field;
      scene()->addItem(field);
    }

}

SquareBoard::~SquareBoard()
{

}

