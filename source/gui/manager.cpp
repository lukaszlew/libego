#include <QDebug>
#include <boost/bind.hpp>
#include <iostream>
#include "manager.h"
#include "GameScene.h"

manager::manager(GameScene *scene, QObject *parent) :
    QObject(parent),
    m_gameScene(scene)
{
  connect(m_gameScene, SIGNAL(mousePressed(int, int, Qt::MouseButtons)), this, SLOT(handleMousePress(int, int, Qt::MouseButtons)));
  newGame();
}


void manager::handleMousePress(int x, int y, Qt::MouseButtons buttons)
{
    std::cout << "click on (" << x << "," << y << ")" << std::endl;
    if (buttons & Qt::LeftButton) {

        bool ok = engine_.Play(Move(current_, 
                gui2vertex(x, y)
            ));
        if (!ok) {
            std::cout << "Invalid move." << std::endl;
            return;
        }

        if (current_==Player::Black()) {
            current_ = Player::White();
        }
        else {
            current_ = Player::Black();
        }
        //refresh
        refreshBoard();
    }
}


void manager::setKomi(double) {
}

void manager::newGame()
{
    std::cout << "newGame()" << std::endl;

    m_gameScene->clearBoard();
    engine_.ClearBoard();
    current_ = Player::Black();
    //emit stateChanged(Player::None());
}

void manager::genMove()
{
    std::cout << "genMove()" << std::endl;
}

void manager::playMove()
{
    std::cout << "playMove()" << std::endl;

    Move move = engine_.Genmove(current_);
    if (!move.IsValid()) {
        std::cout << "Genmove returned invalid move" << std::endl;
        return;
    }
    std::cout << "gen: " << move.GetVertex().ToGtpString() << std::endl;
    int x,y;
    vertex2gui(move.GetVertex(), x ,y);
    std::cout << "play move on (" << x << "," << y << ")" << std::endl;

    if (current_==Player::Black()) {
        current_ = Player::White();
    }
    else {
        current_ = Player::Black();
    }
    bool ok = engine_.Play(move);

    //refresh
    refreshBoard();
}

void manager::undoMove()  {
    std::cout << "undoMove()" << std::endl;
}


void manager::refreshBoard()
{
    std::cout << "refreshBoard()" << std::endl;

    for (int x=1; x<=board_size; x++)
        for (int y=0; y<=board_size; y++) {
            if (!engine_.PlayerAt(gui2vertex(x,y)).IsValid() ) {
                m_gameScene->removeStone(x,y);
            } else {
                if (engine_.PlayerAt(gui2vertex(x,y)) == Player::White()) {
                    m_gameScene->addWhiteStone(x,y);
                }
                if (engine_.PlayerAt(gui2vertex(x,y)) == Player::Black()) {
                    m_gameScene->addBlackStone(x,y);
                }
            }
        }
}

