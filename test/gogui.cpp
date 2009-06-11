#include <QtGui>

#include "gogui.h"
#include "HexBoard.h"
#include "SquareBoard.h"
#include "BoardView.h"

GoGui::GoGui(QWidget *parent) :
  QDialog(parent) {
  m_boardScene = new HexBoard(11);
  BoardView *boardView = new BoardView(m_boardScene, this);

  m_positionEdit = new QLineEdit;
  QLabel *positionLabel = new QLabel("Position");
  positionLabel->setBuddy(m_positionEdit);

  m_labelEdit = new QLineEdit;
  QLabel *labelLabel = new QLabel("Label");
  labelLabel->setBuddy(m_labelEdit);
  m_labelEdit->setEnabled(false);

  m_typeComboBox = new QComboBox;
  QLabel *typeLabel = new QLabel("Type");
  typeLabel->setBuddy(m_typeComboBox);
  m_typeComboBox->addItem("Black Stone");
  m_typeComboBox->addItem("White Stone");
  m_typeComboBox->addItem("Mark");
  m_typeComboBox->addItem("Circle");
  m_typeComboBox->addItem("Square");
  m_typeComboBox->addItem("Triangle");
  m_typeComboBox->addItem("Label");

  QPushButton *addButton = new QPushButton(tr("Add"));
  QPushButton *removeButton = new QPushButton(tr("Remove"));

  QGridLayout *layout = new QGridLayout;
  layout->addWidget(positionLabel, 0, 0);
  layout->addWidget(m_positionEdit, 0, 1, 1, 2);
  layout->addWidget(labelLabel, 1, 0);
  layout->addWidget(m_labelEdit, 1, 1, 1, 2);
  layout->addWidget(typeLabel, 2, 0);
  layout->addWidget(m_typeComboBox, 2, 1, 1, 2);
  layout->addWidget(addButton, 3, 1);
  layout->addWidget(removeButton, 3, 2);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(boardView);
  mainLayout->addLayout(layout);

  connect(m_typeComboBox, SIGNAL(currentIndexChanged(const QString&)), this,
      SLOT(typeIndexChanged(const QString& )));
  connect(addButton, SIGNAL(clicked()), this, SLOT(addButtonClicked()));
  connect(removeButton, SIGNAL(clicked()), this, SLOT(removeButtonClicked()));
  setLayout(mainLayout);
}

GoGui::~GoGui() {

}

void GoGui::typeIndexChanged(const QString& text) {
  m_labelEdit->setEnabled(text == "Label");
}

void GoGui::addButtonClicked() {
  switch (m_typeComboBox->currentIndex()) {
  case 0:
    m_boardScene->addBlackStone(m_positionEdit->text());
    break;
  case 1:
    m_boardScene->addWhiteStone(m_positionEdit->text());
    break;
  case 2:
    m_boardScene->addMark(m_positionEdit->text());
    break;
  case 3:
    m_boardScene->addCircle(m_positionEdit->text());
    break;
  case 4:
    m_boardScene->addSquare(m_positionEdit->text());
    break;
  case 5:
    m_boardScene->addTriangle(m_positionEdit->text());
    break;
  case 6:
    m_boardScene->addLabel(m_positionEdit->text(), m_labelEdit->text());
    break;
  default:
    qDebug() << "wrong combobox current index";
    break;
  }

}

void GoGui::removeButtonClicked() {
  switch (m_typeComboBox->currentIndex()) {
  case 0:
  case 1:
    m_boardScene->removeStone(m_positionEdit->text());
    break;
  case 2:
    m_boardScene->removeMark(m_positionEdit->text());
    break;
  case 3:
    m_boardScene->removeCircle(m_positionEdit->text());
    break;
  case 4:
    m_boardScene->removeSquare(m_positionEdit->text());
    break;
  case 5:
    m_boardScene->removeTriangle(m_positionEdit->text());
    break;
  case 6:
    m_boardScene->removeLabel(m_positionEdit->text());
    break;
  default:
    qDebug() << "wrong combobox current index";
    break;
  }

}

