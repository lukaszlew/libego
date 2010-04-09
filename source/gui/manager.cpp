#include <QCheckBox>
#include <QDebug>
#include <QIntValidator>
#include <QPushButton>
#include <QtGui>

#include <iostream>

#include "GameScene.h"
#include "ResizableView.h"
#include "SquareGrid.h"

#include "engine.hpp"

#include "manager.h"

namespace Gui {

// -----------------------------------------------------------------------------

void vertex2gui (Vertex v, int& x, int& y) {
  x = v.GetColumn ()+1;
  y = board_size-v.GetRow ();
}

Vertex gui2vertex (int x, int y) {
  return Vertex::OfCoords (board_size-y, x-1);
}

// -----------------------------------------------------------------------------

Manager::Manager (Engine& engine) :
  QDialog (0),
  engine (engine)
{
  // Controlls on the right
  QVBoxLayout* controls = new QVBoxLayout;
  QWidget* right = new QWidget (this);
  right->setLayout (controls);

  controls->addWidget(new QLabel("<b>Libego</b>"), 0, Qt::AlignHCenter);

  QPushButton* play_move = new QPushButton("Play move");
  controls->addWidget(play_move);
  connect (play_move, SIGNAL (clicked ()), this, SLOT (playMove ()));

  QPushButton* undo_move = new QPushButton ("Undo move");
  controls->addWidget (undo_move);
  connect (undo_move, SIGNAL (clicked ()), this, SLOT (undoMove ()));

  radio_nul    = new QRadioButton("Nothing", right);
  radio_mcts_n = new QRadioButton("MctsN", right);
  radio_mcts_m = new QRadioButton("MctsMean", right);
  radio_rave_n = new QRadioButton("RaveN", right);
  radio_rave_m = new QRadioButton("RaveMean", right);
  radio_bias   = new QRadioButton("Bias", right);
  controls->addWidget (radio_nul);
  controls->addWidget (radio_mcts_n);
  controls->addWidget (radio_mcts_m);
  controls->addWidget (radio_rave_n);
  controls->addWidget (radio_rave_m);
  controls->addWidget (radio_bias);

  slider1 = new QSlider (this);
  slider2 = new QSlider (this);
  slider1->setOrientation (Qt::Horizontal);
  slider2->setOrientation (Qt::Horizontal);
  slider1->setMinimum (-100);
  slider2->setMinimum (-100);
  slider1->setMaximum (100);
  slider2->setMaximum (100);
  slider1->setValue (0);
  slider2->setValue (0);
  controls->addWidget (slider1);
  controls->addWidget (slider2);
  connect (slider1, SIGNAL (sliderMoved (int)), this, SLOT (sliderMoved (int)));
  connect (slider2, SIGNAL (sliderMoved (int)), this, SLOT (sliderMoved (int)));

  show_gammas = new QCheckBox ("Show gammas");
  controls->addWidget (show_gammas);
  connect (show_gammas, SIGNAL (stateChanged (int)),
           this, SLOT (showGammas (int)));

  
  statebar = new QLabel (" ");
  statebar->setFixedWidth(200);
  QFont font("Monospace");
  font.setStyleHint(QFont::TypeWriter);
  statebar->setFont (font);
  controls->addWidget (statebar);

  controls->addStretch ();

  // Board.
  game_scene = GameScene::createGoScene(9);
  connect (game_scene, SIGNAL (mousePressed (int, int, Qt::MouseButtons)),
           this, SLOT (handleMousePress (int, int, Qt::MouseButtons)));
  connect (game_scene, SIGNAL (hooverEntered (int, int)),
           this, SLOT (handleHooverEntered (int, int)));


  // Main layout

  QHBoxLayout* mainLayout = new QHBoxLayout;
  QSplitter* main_widget = new QSplitter (this);
  mainLayout->addWidget(main_widget);
  main_widget->addWidget (new ResizableView(game_scene, this));
  main_widget->addWidget (right);
  

  setLayout (mainLayout);
  refreshBoard ();
}


void Manager::handleMousePress (int x, int y, Qt::MouseButtons buttons)
{
  if (! (buttons & Qt::LeftButton)) return;
  Vertex v = gui2vertex (x, y);
  engine.Play (Move (engine.GetBoard ().ActPlayer (), v));
  refreshBoard ();
}


void Manager::handleHooverEntered (int x, int y) {
  QString hoover_text = engine.GetStringForVertex (gui2vertex (x, y)).c_str ();
  statebar->setText (hoover_text);
}

void Manager::sliderMoved (int) {
  refreshBoard ();
}

void Manager::showGammas (int state)
{
  refreshBoard ();
}


void Manager::playMove ()
{
  engine.Genmove (engine.GetBoard ().ActPlayer ());
  refreshBoard ();
}


void Manager::undoMove ()
{
  engine.Undo ();
  refreshBoard ();
}


void Manager::refreshBoard ()
{
  std::cout << "refreshBoard ()" << std::endl;

  NatMap <Vertex, double> influence;

  if (show_gammas->checkState() == Qt::Checked) {
    Engine::InfluenceType type;
    if (radio_mcts_n->isChecked()) type = Engine::MctsN;
    if (radio_mcts_m->isChecked()) type = Engine::MctsMean;
    if (radio_rave_n->isChecked()) type = Engine::RaveN;
    if (radio_rave_m->isChecked()) type = Engine::RaveMean;
    if (radio_bias->isChecked())   type = Engine::Bias;
    
    engine.GetInfluence (type, influence);
  }

  for (uint x=1; x<=board_size; x++) {
    for (uint y=1; y<=board_size; y++) {
      Vertex v = gui2vertex (x,y);
      Color col = engine.GetBoard ().ColorAt (v);

      if (col == Color::Empty ()) {
        game_scene->removeStone (x,y);
      } else if (col == Color::White ()) {
        game_scene->addWhiteStone (x,y);
      } else if (col == Color::Black ()) {
        game_scene->addBlackStone (x,y);
      }

      if (engine.GetBoard ().LastVertex () == v) {
        game_scene->addCircle (x,y);
      } else {
        game_scene->removeCircle (x,y);
      }

      //show gammas
      double scale = pow (4, slider1->value() / 100.0);
      double val = influence [v] * scale;
      val = max (val, -1.0);
      val = min (val, 1.0);
      if (show_gammas->checkState() == Qt::Checked && !isnan (val)) {
        QColor color;
        color.setHsvF ((val + 1) / 6.0, 1.0, 1.0, 0.6);
        game_scene->addBGMark (x, y, color);
        //game_scene->addLabel (x, y, QString::number (val, 'f', 2));
      } else {
        game_scene->removeBGMark (x, y);
      }
    }
  }
}

} // namespace
