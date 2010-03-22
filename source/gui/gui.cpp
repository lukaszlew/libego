#include <QtGui>
#include <QApplication>

#include "game_inc.h"
#include "gui.h"


boost::shared_ptr<QtSingleton> QtSingleton::q = boost::shared_ptr<QtSingleton>();


void Gui::Run(Gtp::Io&)
{
    std::cout << "Gui::Run" << std::endl;
    QtSingleton::getInstance().Init();

    GoGui gogui(QtSingleton::getInstance().getEngine());
    gogui.show();

    int ret = QtSingleton::getInstance().Exec();

    std::cout << "Gui::Exit with " << ret << std::endl;
}

