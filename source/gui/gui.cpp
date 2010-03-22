#include <QtGui>
#include <QApplication>

#include "game_inc.h"
#include "gui.h"


void Gui::Run(Gtp::Io&)
{
    std::cout << "Gui::Run" << std::endl;
    QtSingleton::getInstance().Init();

    GoGui gogui;
    gogui.show();

    int ret = QtSingleton::getInstance().Exec();

    std::cout << "Gui::Exit with " << ret << std::endl;
}

