#ifndef GOGUI_H
#define GOGUI_H

#include <QDialog>

class BoardView;
class BoardScene;
class QLineEdit;
class QPushButton;
class QComboBox;

class GoGui: public QDialog
{
Q_OBJECT

public:
  GoGui(QWidget *parent = 0);
  ~GoGui();

private slots:
  void typeIndexChanged(const QString& text);
  void addButtonClicked();
  void removeButtonClicked();

private:

  /*
  BoardView *m_boardView;

  QLineEdit *m_shapeEdit;
  QLineEdit *m_filedEdit;
  QPushButton *m_addButton;
  QPushButton *m_removeButton;
  //*/

  BoardScene *m_boardScene;
  QLineEdit* m_positionEdit;
  QLineEdit* m_labelEdit;
  QComboBox *m_typeComboBox;

};

#endif // GOGUI_H
