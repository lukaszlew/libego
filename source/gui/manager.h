#ifndef MANAGER_H_
#define MANAGER_H_

#include <QObject>
#include <QString>
#include <QDialog>
#include <QLabel>


class GameScene;
class Engine;
class Move;
class Player;

class Manager : public QDialog {
public:
  Manager (Engine& engine);

  void refreshBoard ();

public slots:
  void handleMousePress (int x, int y, Qt::MouseButtons buttons);
  void handleHooverEntered (int x, int y);
  void playMove ();
  void undoMove ();
  void showGammas (int);

signals:
  void stateChanged (const Player&);
  void statusChanged (QString);

private:
  QLabel* statebar;
  GameScene* game_scene;
  Engine& engine;

private:
  Q_OBJECT
};

#endif /* MANAGER_H_ */
