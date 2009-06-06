#include "Grid.h"

Grid::Grid(int size, QGraphicsItem * parent) :
  QGraphicsItem(parent), m_size(size)
{
  //TODO setZIndex(...)
}

Grid::~Grid()
{
}
