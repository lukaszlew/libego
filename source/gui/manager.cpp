#include <QDebug>
#include <boost/bind.hpp>
#include <iostream>
#include "manager.h"
#include "GameScene.h"

Manager::Manager (Engine& engine, GameScene *scene, QObject *parent) :
  QObject (parent),
  m_gameScene (scene),
  engine_ (engine),
  foreignEngine_ (false)
{
  connect (m_gameScene, SIGNAL (mousePressed (int, int, Qt::MouseButtons)),
           this, SLOT (handleMousePress (int, int, Qt::MouseButtons)));
  connect (m_gameScene, SIGNAL (hooverEntered (int, int)), this,
           SLOT (handleHooverEntered (int, int)));
  //newGame ();
}


void Manager::setForeignEngine ()
{
  foreignEngine_ = true;

  current_ = Player::Black (); //TODO, get real current player

  refreshBoard ();
}


Manager::~Manager ()
{
  if (!foreignEngine_)
    delete &engine_;
}


void Manager::handleMousePress (int x, int y, Qt::MouseButtons buttons)
{
  std::cout << "click on (" << x << "," << y << ")" << std::endl;
  if (buttons & Qt::LeftButton) {

    bool ok = engine_.Play (Move (current_, 
                                  gui2vertex (x, y)
                                  ));
    if (!ok) {
      std::cout << "Invalid move." << std::endl;
      return;
    }

    if (current_==Player::Black ()) {
      current_ = Player::White ();
    }
    else {
      current_ = Player::Black ();
    }
    //refresh
    refreshBoard ();
  }
}


void Manager::handleHooverEntered (int x, int y)
{
  //std::cout << "Text for (" << x << ", " << y << "): "
  //    << engine_.GetStringForVertex (gui2vertex (x, y))
  //    << std::endl;

  emit statusChanged (QString (engine_.GetStringForVertex (gui2vertex (x, y)).c_str ()));
}


void Manager::showGammas (int state)
{
  std::cout << "showGammas () " << (state ? "TRUE" : "FALSE") << std::endl;

  if (state) {
    //show gammas
    for (uint x=1; x<=board_size; x++)
      for (uint y=1; y<=board_size; y++) {
        double val = engine_.GetStatForVertex (gui2vertex (x, y));
        if (-1<=val && val<=1) {
          QColor qc (
                     (val>0) ? (255* (1-val)) : 255,
                     (val>0) ? 255 : - (255*val),
                     0, 80);
          m_gameScene->addBGMark (x, y, qc);
          //m_gameScene->addLabel (x, y, QString::number (val, 'f', 2));
        }
      }
  }
  else {
    //clear backgrounds
    for (uint x=1; x<=board_size; x++)
      for (uint y=1; y<=board_size; y++) {
        m_gameScene->removeBGMark (x, y);
      }
  }
}


void Manager::setKomi (double) {
}


void Manager::newGame ()
{
  std::cout << "newGame ()" << std::endl;

  m_gameScene->clearBoard ();
  engine_.ClearBoard ();
  current_ = Player::Black ();
  //emit stateChanged (Player::None ());
}


void Manager::genMove ()
{
  std::cout << "genMove ()" << std::endl;
}


void Manager::playMove ()
{
  std::cout << "playMove ()" << std::endl;

  Move move = engine_.Genmove (current_);
  if (!move.IsValid ()) {
    std::cout << "Genmove returned invalid move" << std::endl;
    return;
  }
  std::cout << "gen: " << move.GetVertex ().ToGtpString () << std::endl;
  int x,y;
  vertex2gui (move.GetVertex (), x ,y);
  std::cout << "play move on (" << x << "," << y << ")" << std::endl;

  if (current_==Player::Black ()) {
    current_ = Player::White ();
  }
  else {
    current_ = Player::Black ();
  }
  bool ok = engine_.Play (move);
  unused (ok);

  //refresh
  refreshBoard ();
}


void Manager::undoMove ()  {
  std::cout << "undoMove ()" << std::endl;
  bool ok = engine_.Undo ();
  if (ok) {
    std::cout << "One move back, replayed." << std::endl;
  }
  else {
    std::cout << "Cannot undo." << std::endl;
  }
  //refresh
  refreshBoard ();
}


void Manager::refreshBoard ()
{
  std::cout << "refreshBoard ()" << std::endl;

  for (uint x=1; x<=board_size; x++)
    for (uint y=1; y<=board_size; y++) {
      if (!engine_.PlayerAt (gui2vertex (x,y)).IsValid () ) {
        m_gameScene->removeStone (x,y);
        m_gameScene->removeCircle (x,y);
      } else {
        if (engine_.PlayerAt (gui2vertex (x,y)) == Player::White ()) {
          m_gameScene->addWhiteStone (x,y);
        }
        if (engine_.PlayerAt (gui2vertex (x,y)) == Player::Black ()) {
          m_gameScene->addBlackStone (x,y);
        }
        if (engine_.LastVertex () == gui2vertex (x,y)) {
          m_gameScene->addCircle (x,y);
        }
        else {
          m_gameScene->removeCircle (x,y);
        }
      }
    }
}
