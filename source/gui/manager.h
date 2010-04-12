#ifndef MANAGER_H_
#define MANAGER_H_

#include <QObject>
#include <QString>
#include <QDialog>
#include <QLabel>
#include <QSlider>
#include <QRadioButton>

#include <nat.hpp>
#include <vertex.hpp>
#include <vertex.hpp>

class GameScene;
class QCheckBox;
class Engine;
class Move;
class Player;

namespace Gui {

class Manager : public QDialog {
public:
  Manager (Engine& engine);


public slots:
  void getInfluence ();
  void refreshBoard ();
  void sliderMoved (int value);
  void handleMousePress (int x, int y, Qt::MouseButtons buttons);
  void handleHooverEntered (int x, int y);
  void clearBoard ();
  void playMove ();
  void undoMove ();
  void playoutMove ();
  void showGammas (int);

private:

  QRadioButton* radio_nul;
  QRadioButton* radio_mcts_n;
  QRadioButton* radio_mcts_m;
  QRadioButton* radio_rave_n;
  QRadioButton* radio_rave_m;
  QRadioButton* radio_bias;
  QRadioButton* radio_mix;
  QRadioButton* radio_samp_p;
  QRadioButton* radio_gamma;
  QRadioButton* radio_gamma2;

  QSlider* slider1;
  QSlider* slider2;

  QLabel* statebar;

  GameScene* game_scene;
  NatMap <Vertex, double> influence;

  Engine& engine;

private:
  Q_OBJECT
};

} // namespace

#endif
