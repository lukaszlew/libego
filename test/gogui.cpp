#include <QtGui>
#include <QIntValidator>

#include "gogui.h"
#include "manager.h"
#include "BoardScene.h"
#include "BoardView.h"
#include "SquareGrid.h"
#include "HexGrid.h"
#include "YGrid.h"
#include "HoGrid.h"
#include "HavannahGrid.h"

GoGui::GoGui(QWidget *parent) :
  QDialog(parent) {
  Grid *grid = new SquareGrid(11);
  BoardScene *boardScene = new BoardScene(grid);
  BoardView *boardView = new BoardView(boardScene, this);
  manager *m = new manager(boardScene, this);

  labelEdit = new QLineEdit;
  QLabel *labelLabel = new QLabel("Label");
  labelLabel->setBuddy(labelEdit);
  labelEdit->setEnabled(false);

  QComboBox *typeComboBox = new QComboBox;
  QLabel *typeLabel = new QLabel("Type");
  typeLabel->setBuddy(typeComboBox);
  typeComboBox->addItem("Black Stone");
  typeComboBox->addItem("White Stone");
  typeComboBox->addItem("Mark");
  typeComboBox->addItem("Circle");
  typeComboBox->addItem("Square");
  typeComboBox->addItem("Triangle");
  typeComboBox->addItem("Label");

  QGridLayout *layout = new QGridLayout;
  layout->addWidget(labelLabel, 0, 0);
  layout->addWidget(labelEdit, 0, 1);
  layout->addWidget(typeLabel, 1, 0);
  layout->addWidget(typeComboBox, 1, 1);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(boardView);
  mainLayout->addLayout(layout);

  connect(typeComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(typeIndexChanged(const QString& )));
  connect(typeComboBox, SIGNAL(currentIndexChanged(int)), m, SLOT(setIndex(int)));
  connect(labelEdit, SIGNAL(textChanged(const QString &)), m, SLOT(setLabel(const QString&)));
  setLayout(mainLayout);
}

void GoGui::typeIndexChanged(const QString& text) {
  labelEdit->setEnabled(text == "Label");
}

