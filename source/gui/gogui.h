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
  GoGui(Engine& engine, QWidget *parent = 0);

private:
    void initView(bool, Engine&);

private slots:
    void setKomi();
    void setWinner(const Player&);
    void setStatus(QString);

private:
  manager *m;
  QLabel *statebar;
};

#endif // GOGUI_H
