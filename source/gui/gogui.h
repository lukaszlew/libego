#ifndef GOGUI_H
#define GOGUI_H

#include <QDialog>
#include <QLabel>
#include "manager.h"

class QLineEdit;

class GoGui: public QDialog {
public:
  GoGui(Engine& engine, QWidget *parent = 0);

private:
  void initView(Engine&);

private slots:
  void setKomi();
  void setWinner(const Player&);
  void setStatus(QString);

private:
  Manager *m;
  QLabel *statebar;

private:
  Q_OBJECT
};

#endif // GOGUI_H
