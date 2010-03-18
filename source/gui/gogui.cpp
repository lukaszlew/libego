#include <QtGui>
#include <QIntValidator>

#include "gogui.h"
#include "GameScene.h"
#include "ResizableView.h"
#include "SquareGrid.h"

GoGui::GoGui(QWidget *parent) :
  QDialog(parent) {
  GameScene *gameScene = GameScene::createGoScene(9);
  ResizableView *gameView = new ResizableView(gameScene, this);
  m = new manager(gameScene, this);

//  labelEdit = new QLineEdit;
//  QLabel *labelLabel = new QLabel("Label");
//  labelLabel->setBuddy(labelEdit);
//  labelEdit->setEnabled(false);
//
//  QComboBox *typeComboBox = new QComboBox;
//  QLabel *typeLabel = new QLabel("Type");
//  typeLabel->setBuddy(typeComboBox);
//  typeComboBox->addItem("Black Stone");
//  typeComboBox->addItem("White Stone");
//  typeComboBox->addItem("Mark");
//  typeComboBox->addItem("Circle");
//  typeComboBox->addItem("Square");
//  typeComboBox->addItem("Triangle");
//  typeComboBox->addItem("Label");
//
//  QGridLayout *layout = new QGridLayout;
//  layout->addWidget(labelLabel, 0, 0);
//  layout->addWidget(labelEdit, 0, 1);
//  layout->addWidget(typeLabel, 1, 0);
//  layout->addWidget(typeComboBox, 1, 1);
//
//  QVBoxLayout *mainLayout = new QVBoxLayout;
//  mainLayout->addWidget(boardView);
//  mainLayout->addLayout(layout);
//
//  connect(typeComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(typeIndexChanged(const QString& )));
//  connect(typeComboBox, SIGNAL(currentIndexChanged(int)), m, SLOT(setIndex(int)));
//  connect(labelEdit, SIGNAL(textChanged(const QString &)), m, SLOT(setLabel(const QString&)));
//  setLayout(mainLayout);

  QPushButton *new_game = new QPushButton("New game");
  QPushButton *gen_move = new QPushButton("Gen move");

  QPushButton *play_move = new QPushButton("Play move");
  QPushButton *undo_move = new QPushButton("Undo move");

  QPushButton *set_komi = new QPushButton("Set komi");

  QPushButton *quit = new QPushButton("Quit");

  QVBoxLayout *controls = new QVBoxLayout;
  controls->addWidget(new QLabel("<b>GoGui</b>"), 0, Qt::AlignHCenter);
  //controls->addSpacing(10);
  controls->addWidget(new_game);
  controls->addWidget(gen_move);
  controls->addWidget(play_move);
  controls->addWidget(undo_move);
  controls->addWidget(set_komi);
  controls->addWidget(quit);
  controls->addStretch();

  QHBoxLayout *mainLayout = new QHBoxLayout;
  mainLayout->addWidget(gameView);
  mainLayout->addLayout(controls);

  connect(new_game, SIGNAL(clicked()), m, SLOT(newGame()));
  connect(gen_move, SIGNAL(clicked()), m, SLOT(genMove()));
  connect(play_move, SIGNAL(clicked()), m, SLOT(playMove()));
  connect(undo_move, SIGNAL(clicked()), m, SLOT(undoMove()));
  connect(set_komi, SIGNAL(clicked()), this, SLOT(setKomi()));
  connect(quit, SIGNAL(clicked()), this, SLOT(close()));

  connect(m, SIGNAL(stateChanged(const Player&)), this, SLOT(setWinner(const Player&)));

  statebar = new QLabel();
  QVBoxLayout *all_ = new QVBoxLayout;
  all_->addLayout(mainLayout);
  all_->addWidget(statebar);

  setLayout(all_);
}

//void GoGui::typeIndexChanged(const QString& text) {
//  labelEdit->setEnabled(text == "Label");
//}

void GoGui::setKomi() {
    bool ok;
    double komi = QInputDialog::getDouble(
            this, "Set komi", "Enter double:", 
            10, 0, 100, 2, &ok);
    if (ok)
        m->setKomi(komi);
}

void GoGui::setWinner(const Player&) {
}
