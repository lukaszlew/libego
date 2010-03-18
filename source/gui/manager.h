#ifndef MANAGER_H_
#define MANAGER_H_

#include <QObject>
#include <QString>
#include "game_inc.h"

class GameScene;

inline void vertex2gui(Vertex v, int& x, int& y)
{
    x = v.GetColumn()+1;
    y = board_size-v.GetRow();
}

inline Vertex gui2vertex(int x, int y)
{
    return Vertex::OfCoords(board_size-y, x-1);
}

class manager : public QObject
{
  Q_OBJECT
public:
  manager(GameScene *scene, QObject * parent = 0);

  //void handleLeftClick(int x, int y);
  //void handleRightClick(int x, int y);

//public slots:
//  void setIndex(int index) { m_index = index; }
//  void setLabel(const QString& label) { m_label = label; }

private:
    bool putStone(const Move&);
    void refreshBoard();

public slots:
    void handleMousePress(int x, int y, Qt::MouseButtons buttons);
    void setKomi(double);
    void newGame();
    void genMove();
    void playMove();
    void undoMove();

signals:
    void stateChanged(const Player&);

private:
//  int m_index;
//  QString m_label;
  GameScene *m_gameScene;
  Engine engine_;
  Player current_;
};

#endif /* MANAGER_H_ */
