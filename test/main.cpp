#include <QtGui>
#include <QApplication>

#include "BoardView.h"
#include "SquareBoard.h"
#include "HexBoard.h"
#include "gogui.h"

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(images);

  QApplication a(argc, argv);

  //SquareBoard scene(19);
  //scene.addBlackStone("1-1");
  //BoardView view(&scene);
  //view.show();
  GoGui gogui;
  gogui.show();
  return a.exec();
}
