#include <QtGui>
#include <QPushButton>
#include <QCheckBox>
#include <QIntValidator>

#include "gogui.h"
#include "GameScene.h"
#include "ResizableView.h"
#include "SquareGrid.h"

GoGui::GoGui(Engine& engine, QWidget *parent ) :
  QDialog(parent)
{
    game_scene = GameScene::createGoScene(9);

    manager = new Manager (engine, game_scene, this);

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

    connect(play_move, SIGNAL(clicked()), manager, SLOT(playMove()));
    connect(undo_move, SIGNAL(clicked()), manager, SLOT(undoMove()));
    connect(show_gammas, SIGNAL(stateChanged(int)), manager, SLOT(showGammas(int)));
    connect(quit, SIGNAL(clicked()), this, SLOT(close()));

    connect(manager, SIGNAL(statusChanged(QString)), this, SLOT(setStatus(QString)));

    statebar = new QLabel(" ");
    QVBoxLayout *all_ = new QVBoxLayout;
    all_->addLayout(mainLayout);
    all_->addWidget(statebar);

    setLayout(all_);
    manager->refreshBoard ();
}

void GoGui::setStatus(QString s) {
    statebar->setText(s);
}
