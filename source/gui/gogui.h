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

private slots:
    void setKomi();
    void setWinner(const Player&);

private:
  manager *m;
  QLabel *statebar;
};

#endif // GOGUI_H
