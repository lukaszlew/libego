#include "SquareBoard.h"
#include "SquareField.h"
#include "SquareGrid.h"

SquareBoard::SquareBoard(int size, QObject *parent) :
  BoardScene(size, parent)
{
  addGrid();

  for (int x = 1; x <= size; x++)
    for (int y = 1; y <= size; y++) {
      Field *field = new SquareField(x, y);
      m_fields.insert(getFieldString(x, y), field);
      addItem(field);
    }
}

SquareBoard::~SquareBoard()
{
}

QGraphicsItem* SquareBoard::addGrid()
{
  m_grid = new SquareGrid(m_size);
  addItem(m_grid);
  return m_grid;
}

QGraphicsItem* SquareBoard::addRuler()
{
  //TODO
  return NULL;
}


