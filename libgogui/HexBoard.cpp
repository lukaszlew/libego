#include "HexBoard.h"
#include "HexField.h"
#include "HexGrid.h"

HexBoard::HexBoard(int size, QObject *parent) :
  BoardScene(size, parent)
{
  addGrid();
  for (int x = m_grid->minimalCoordinate(); x <= m_grid->maximalCoordinate(); x++) {
    for (int y = m_grid->minimalCoordinate(x); y <= m_grid->maximalCoordinate(x); y++) {
      Field *field = new HexField(x, y);
      m_fields.insert(getFieldString(x, y), field);
      field->setPos(getFieldPosition(x, y));
      addItem(field);
    }
  }
}

QGraphicsItem* HexBoard::addGrid() {
  m_grid = new HexGrid(m_size);
  addItem(m_grid);
  return m_grid;
}

QGraphicsItem* HexBoard::addRuler() {
  //TODO
  return NULL;
}

QPointF HexBoard::getFieldPosition(int x, int y) {
  return QPointF((x + 0.5 * y) * HexField::s_width, y * 1.5 * HexField::s_height);
}
