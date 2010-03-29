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
  game_scene = GameScene::createGoScene(9);

  ResizableView *gameView = new ResizableView(game_scene, this);
  QPushButton *play_move = new QPushButton("Play move");
  QPushButton *undo_move = new QPushButton("Undo move");
  QCheckBox *show_gammas = new QCheckBox("Show gammas");
  QPushButton *quit = new QPushButton("Quit");
  QVBoxLayout *controls = new QVBoxLayout;
  
  controls->addWidget(new QLabel("<b>Libego</b>"), 0, Qt::AlignHCenter);
  controls->addWidget(play_move);
  controls->addWidget(undo_move);
  controls->addWidget(show_gammas);
  controls->addWidget(quit);
  controls->addStretch();
  
  QHBoxLayout *mainLayout = new QHBoxLayout;
  mainLayout->addWidget(gameView);
  mainLayout->addLayout(controls);

  connect(play_move, SIGNAL(clicked()), this, SLOT(playMove()));
  connect(undo_move, SIGNAL(clicked()), this, SLOT(undoMove()));
  connect(show_gammas, SIGNAL(stateChanged(int)), this, SLOT(showGammas(int)));
  connect(quit, SIGNAL(clicked()), this, SLOT(close()));

  statebar = new QLabel(" ");
  QVBoxLayout *all = new QVBoxLayout;
  all->addLayout(mainLayout);
  all->addWidget(statebar);

  connect (game_scene, SIGNAL (mousePressed (int, int, Qt::MouseButtons)),
           this, SLOT (handleMousePress (int, int, Qt::MouseButtons)));
  connect (game_scene, SIGNAL (hooverEntered (int, int)), this,
           SLOT (handleHooverEntered (int, int)));

  setLayout(all);
  refreshBoard ();
}


void Manager::handleMousePress (int x, int y, Qt::MouseButtons buttons)
{
  if (!(buttons & Qt::LeftButton)) return;
  Vertex v = gui2vertex (x, y);
  engine.Play (Move (engine.GetBoard().ActPlayer (), v));
  refreshBoard ();
}


void Manager::handleHooverEntered (int x, int y) {
  statebar->setText (QString (engine.GetStringForVertex (gui2vertex (x, y)).c_str ()));
}


void Manager::showGammas (int state)
{
  std::cout << "showGammas () " << (state ? "TRUE" : "FALSE") << std::endl;

  if (state) {
    //show gammas
    for (uint x=1; x<=board_size; x++)
      for (uint y=1; y<=board_size; y++) {
        double val = engine.GetStatForVertex (gui2vertex (x, y));
        if (-1<=val && val<=1) {
          QColor qc (
                     (val>0) ? (255* (1-val)) : 255,
                     (val>0) ? 255 : - (255*val),
                     0, 80);
          game_scene->addBGMark (x, y, qc);
          //game_scene->addLabel (x, y, QString::number (val, 'f', 2));
        }
      }
  } else {
    //clear backgrounds
    for (uint x=1; x<=board_size; x++)
      for (uint y=1; y<=board_size; y++) {
        game_scene->removeBGMark (x, y);
      }
  }
}


void Manager::playMove ()
{
  Move move = engine.Genmove (engine.GetBoard().ActPlayer());
  CHECK (engine.Play (move));
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
      Color col = engine.GetBoard().ColorAt (v);

      if (col == Color::Empty ()) {
        game_scene->removeStone (x,y);
      } else if (col == Color::White ()) {
        game_scene->addWhiteStone (x,y);
      } else if (col == Color::Black ()) {
        game_scene->addBlackStone (x,y);
      }

      if (engine.GetBoard().LastVertex () == v) {
        game_scene->addCircle (x,y);
      } else {
        game_scene->removeCircle (x,y);
      }
    }
  }
}
