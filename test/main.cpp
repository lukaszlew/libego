#include <QtGui>
#include <QApplication>

#include "BoardView.h"
#include "SquareBoard.h"
#include "HexBoard.h"

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(images);

  QApplication a(argc, argv);

  HexBoard scene(8);
  BoardView view(&scene);
  view.show();
  return a.exec();
}
