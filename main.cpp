#include <QtGui>
#include <QApplication>

#include "gogui.h"

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(images);

  QApplication a(argc, argv);
  GoGui w;
  w.show();
  return a.exec();
}
