#ifndef BOARD_H
#define BOARD_H

#include <QGraphicsView>
#include <QVector>

class Field;

class Board: public QGraphicsView
{
Q_OBJECT

public:
  Board(int size, QWidget *parent = 0);
  ~Board();

  void wheelEvent(QWheelEvent *event);

  /* jezeli bedzie to wygodniejsze to metody insert moga zwracac np QGraphicsItem*
   * wtedy bylaby dodatokowo metoda remove(QGraphicsItems*)
   */
  virtual void insertBlackStone(const QString& pos);
  virtual void insertWhiteStone(const QString& pos);
  virtual void removeStone(const QString& pos);

  // ustawienie na polu cyfry lub liczby
  virtual void markField(const QString& pos, char c);

  virtual void insertCircle(const QString& pos, QColor color);
  virtual void removeCircle(const QString& pos);

  virtual void insertTriangle(const QString& pos, QColor color);
  virtual void removeTriangle(const QString& pos);
  /* ... kolejne metody wstawiajace rozne dziwne rzeczy na polu*/

  virtual void drawGrid();

protected:
  // pewnie trzeba bedzie to zmienic na mape QString-Field
  QVector<Field*> m_fields;
  QGraphicsItem *m_grid;

};

#endif // BOARD_H
