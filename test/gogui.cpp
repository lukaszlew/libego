#include <QtGui>

#include "gogui.h"
#include "SquareBoard.h"
#include "BoardView.h"

GoGui::GoGui(QWidget *parent) :
  QDialog(parent)
{
  m_boardScene = new SquareBoard(9);
  m_boardView = new BoardView(m_boardScene, this);
  m_filedEdit = new QLineEdit;
  m_shapeEdit = new QLineEdit;
  m_addButton = new QPushButton(tr("Add"));
  m_removeButton = new QPushButton(tr("Remove"));

  QHBoxLayout *buttons = new QHBoxLayout;
  buttons->addWidget(m_addButton);
  buttons->addWidget(m_removeButton);

  QVBoxLayout *dialogLayout = new QVBoxLayout;
  dialogLayout->addWidget(m_shapeEdit);
  dialogLayout->addWidget(m_filedEdit);
  dialogLayout->addLayout(buttons);
  dialogLayout->setSizeConstraint(QLayout::SetMinimumSize);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(m_boardView);
  mainLayout->addLayout(dialogLayout);

  connect(m_removeButton, SIGNAL(clicked()), m_shapeEdit, SLOT(clear()));

  setLayout(mainLayout);
}

GoGui::~GoGui()
{

}

