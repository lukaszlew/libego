#include "SquareBoard.h"
#include "SquareField.h"
#include "SquareGrid.h"
#include "Ruler.h"

SquareBoard::SquareBoard(int size, QObject *parent) :
  BoardScene(size, parent) {
  addGrid();
  addRuler();
  for (int x = m_grid->minimalCoordinate(); x <= m_grid->maximalCoordinate(); x++) {
    for (int y = m_grid->minimalCoordinate(x); y <= m_grid->maximalCoordinate(x); y++) {
      Field *field = new SquareField(x, y);
      m_fields.insert(getFieldString(x, y), field);
      field->setPos(getFieldPosition(x, y));
      addItem(field);
    }
  }
}

QPointF SquareBoard::getFieldPosition(int x, int y) {
  return QPointF(x * SquareField::s_width, y * SquareField::s_height);
}

QGraphicsItem* SquareBoard::addGrid() {
  m_grid = new SquareGrid(m_size);
  addItem(m_grid);
  return m_grid;
}

QGraphicsItem* SquareBoard::addRuler() {
  m_ruler = new Ruler(Ruler::LocateBefore | Ruler::LocateAfter, Ruler::LocateAfter | Ruler::LocateBefore
      | Ruler::TypeLetters, m_grid);
  emit sceneRectChanged(sceneRect());
  return m_ruler;
}
