#ifndef GOGUI_H
#define GOGUI_H

#include <QDialog>
#include <QLabel>
#include "manager.h"

class QLineEdit;

class GoGui: public QDialog {
public:
  GoGui(Engine& engine, QWidget *parent = 0);

private slots:
  void setStatus(QString);

private:
  Manager* manager;
  QLabel* statebar;
  GameScene* game_scene;

private:
  Q_OBJECT
};

#endif // GOGUI_H
