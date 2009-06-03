#ifndef GOGUI_H
#define GOGUI_H

#include <QDialog>

class Board;
class QLineEdit;
class QPushButton;

class GoGui: public QDialog
{
Q_OBJECT

public:
  GoGui(QWidget *parent = 0);
  ~GoGui();

private:
  Board *m_board;
  QLineEdit *m_shapeEdit;
  QLineEdit *m_filedEdit;
  QPushButton *m_addButton;
  QPushButton *m_removeButton;

};

#endif // GOGUI_H
