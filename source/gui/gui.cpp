#include <QtGui>
#include <QApplication>
#include "gui.h"
#include "engine.hpp"
#include "manager.h"


void RunGui (Engine& engine) {
  Q_INIT_RESOURCE(images);
  
  int argc = 1;
  char** argv = (char**)malloc(sizeof(char*));
  argv[0] = (char*)malloc(6*sizeof(char));
  strncpy (argv[0], "GoGui\0", 6);
  QApplication app (argc, argv);
  Gui::Manager gogui (engine);
  gogui.show();
  app.exec();
}
