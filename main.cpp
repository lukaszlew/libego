#include <QtGui>
#include <QApplication>

#include "BoardView.h"
#include "SquareBoard.h"

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(images);

  QApplication a(argc, argv);

  SquareBoard scene(19);
  BoardView view(&scene);
  view.show();
  return a.exec();
}
