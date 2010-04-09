#ifndef MANAGER_H_
#define MANAGER_H_

#include <QObject>
#include <QString>
#include <QDialog>
#include <QLabel>
#include <QSlider>


class GameScene;
class QCheckBox;
class Engine;
class Move;
class Player;

namespace Gui {

class Manager : public QDialog {
public:
  Manager (Engine& engine);

  void refreshBoard ();

public slots:
  void sliderMoved (int value);
  void handleMousePress (int x, int y, Qt::MouseButtons buttons);
  void handleHooverEntered (int x, int y);
  void playMove ();
  void undoMove ();
  void showGammas (int);

private:
  QSlider* slider1;
  QSlider* slider2;
  QLabel* statebar;
  GameScene* game_scene;
  QCheckBox* show_gammas;
  Engine& engine;

private:
  Q_OBJECT
};

} // namespace

#endif
