#ifndef GO_GUI_H_
#define GO_GUI_H_

#include <QApplication>
#include <boost/shared_ptr.hpp>
#include "engine.hpp"
#include "gogui.h"


class QtSingleton
{
private:
    QtSingleton() :
        app_(NULL)
    {
        Q_INIT_RESOURCE(images);
    }
public:
    static QtSingleton& getInstance()
    {
        static boost::shared_ptr<QtSingleton> q;
        if (!q.get()) {
            q.reset(new QtSingleton());
        }
        return *q;
    }
public:
    static void Init()
    {
        if (getInstance().app_) {
            throw "QApplication already initialized";
        }
        int argc = 1;
        char **argv;
        argv = (char**)malloc(sizeof(char**));
        argv[0] = (char*)malloc(sizeof(char*));

        getInstance().app_ = new QApplication(argc, argv);

        free(argv[0]);
        free(argv);
    }
    static int Exec()
    {
        if (!getInstance().app_) {
            throw "QApplication not initialized";
        }
        int ret = getInstance().app_->exec();
        getInstance().app_ = NULL;

        return ret;
    }
private:
    QApplication *app_;
};


class Gui
{
public:
    Gui(Engine& engine)
    {
        gtp.Register("gui", Gui::Run);
    }

    static void Run(Gtp::Io&);
};

#endif /* GO_GUI_H_ */
