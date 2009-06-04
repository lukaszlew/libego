#include "HexBoard.h"

#include "HexGrid.h"

HexBoard::HexBoard(int size, QObject *parent) :
  BoardScene(size, parent)
{
  //TODO

}

HexBoard::~HexBoard()
{
}

QGraphicsItem* HexBoard::addGrid()
{
  m_grid = new HexGrid(m_size);
  addItem(m_grid);
  return m_grid;
}

QGraphicsItem* HexBoard::addRuler()
{
  //TODO
  return NULL;
}
