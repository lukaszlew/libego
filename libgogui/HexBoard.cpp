#include "HexBoard.h"
#include "HexField.h"
#include "HexGrid.h"

HexBoard::HexBoard(int size, QObject *parent) :
  BoardScene(size, parent)
{
  addGrid();

  for (int x = 1; x <= size; x++) {
    int start = size / 2 + 2 - x;
    int end = size + size/2 + 1 - x;
    if (start < 1) start = 1;
    if (end > size) end = size;
    for (int y = start; y <= end; y++) {
      Field *field = new HexField(x, y);
      m_fields.insert(getFieldString(x, y), field);
      addItem(field);
    }
  }
}

HexBoard::~HexBoard()
{
}

QGraphicsItem* HexBoard::addGrid()
{
  /*
  m_grid = new HexGrid(m_size);
  addItem(m_grid);
  //*/
  return m_grid;
}

QGraphicsItem* HexBoard::addRuler()
{
  //TODO
  return NULL;
}
