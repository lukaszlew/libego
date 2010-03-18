#ifndef GOGUI_H
#define GOGUI_H

#include <QDialog>
#include <QLabel>
#include "manager.h"

class QLineEdit;

class GoGui: public QDialog
{
Q_OBJECT

public:
  GoGui(QWidget *parent = 0);

//private slots:
//  void typeIndexChanged(const QString& text);
private slots:
    void setKomi();
    void setWinner(const Player&);

private:
  //QLineEdit *labelEdit;dd
  manager *m;
  QLabel *statebar;
};

#endif // GOGUI_H
