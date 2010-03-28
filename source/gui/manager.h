#ifndef MANAGER_H_
#define MANAGER_H_

#include <QObject>
#include <QString>
#include "game_inc.h"

class GameScene;

inline
void vertex2gui (Vertex v, int& x, int& y) {
  x = v.GetColumn ()+1;
  y = board_size-v.GetRow ();
}

inline
Vertex gui2vertex (int x, int y) {
  return Vertex::OfCoords (board_size-y, x-1);
}


class Manager : public QObject {
public:
  Manager (Engine& engine, GameScene *scene, QObject * parent = 0);

  virtual ~Manager ();

private:
  bool putStone (const Move&);
  void refreshBoard ();

public slots:
  void handleMousePress (int x, int y, Qt::MouseButtons buttons);
  void handleHooverEntered (int x, int y);
  void setKomi (double);
  void newGame ();
  void genMove ();
  void playMove ();
  void undoMove ();
  void showGammas (int);

signals:
  void stateChanged (const Player&);
  void statusChanged (QString);

private:
  GameScene *m_gameScene;
  Engine& engine_;

private:
  Q_OBJECT
};

#endif /* MANAGER_H_ */
