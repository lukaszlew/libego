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
  controls->addWidget(new QLabel("<b>Libego</b>"), 0, Qt::AlignHCenter);

  QPushButton* play_move = new QPushButton("Play move");
  controls->addWidget(play_move);
  connect (play_move, SIGNAL (clicked ()), this, SLOT (playMove ()));

  QPushButton* undo_move = new QPushButton ("Undo move");
  controls->addWidget (undo_move);
  connect (undo_move, SIGNAL (clicked ()), this, SLOT (undoMove ()));

  QCheckBox* show_gammas = new QCheckBox ("Show gammas");
  controls->addWidget (show_gammas);
  connect (show_gammas, SIGNAL (stateChanged (Qt::CheckState)),
           this, SLOT (showGammas (Qt::CheckState)));

  QPushButton* quit = new QPushButton ("Quit");
  controls->addWidget (quit);
  connect (quit, SIGNAL (clicked ()), this, SLOT (close ()));
  
  statebar = new QLabel (" ");
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
  mainLayout->addWidget (new ResizableView(game_scene, this));
  mainLayout->addLayout (controls);

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


void Manager::showGammas (Qt::CheckState state)
{
  std::cout << "showGammas () " << state << std::endl;

  if (state == Qt::Checked) {
    //show gammas
    for (uint x=1; x<=board_size; x++) {
      for (uint y=1; y<=board_size; y++) {
        Vertex v = gui2vertex (x, y);
        double val = engine.GetStatForVertex (v);
        if (-1<=val && val<=1) {
          double r = (val>0) ? (255* (1-val)) : 255;
          double g = (val>0) ? 255 : - (255*val);
          double b = 0;
          double a = 60;
          game_scene->addBGMark (x, y, QColor (r, g, b, a));
          //game_scene->addLabel (x, y, QString::number (val, 'f', 2));
        }
      }
    }
  } else {
    //clear backgrounds
    for (uint x=1; x<=board_size; x++) {
      for (uint y=1; y<=board_size; y++) {
        game_scene->removeBGMark (x, y);
      }
    }
  }
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
    }
  }
}
