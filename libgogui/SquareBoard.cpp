#include "SquareBoard.h"
#include "SquareField.h"
#include "SquareGrid.h"

SquareBoard::SquareBoard(int size, QObject *parent) :
  BoardScene(size, parent) {
  addGrid();
  for (int x = m_grid->minimalCoordinate(); x <= m_grid->maximalCoordinate(); x++) {
    for (int y = m_grid->minimalCoordinate(x); y <= m_grid->maximalCoordinate(x); y++) {
      Field *field = new SquareField(x, y);
      m_fields.insert(getFieldString(x, y), field);
      field->setPos(getFieldPosition(x, y));
      addItem(field);
    }
  }
}

QGraphicsItem* SquareBoard::addGrid() {
  m_grid = new SquareGrid(m_size);
  addItem(m_grid);
  return m_grid;
}

QGraphicsItem* SquareBoard::addRuler() {
  //TODO
  return NULL;
}

QPointF SquareBoard::getFieldPosition(int x, int y) {
  return QPointF(x * SquareField::s_width, y * SquareField::s_height);
}
