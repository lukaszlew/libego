#ifndef GO_GUI_H_
#define GO_GUI_H_

#include <QApplication>
#include <boost/shared_ptr.hpp>
#include "engine.hpp"
#include "gogui.h"

class Gui;

class QtSingleton
{
friend class Gui;
private:
    QtSingleton(Engine& engine) :
        app_(NULL),
        engine_(engine)
    {
        Q_INIT_RESOURCE(images);
    }
private:

    static boost::shared_ptr<QtSingleton> q;

    static void setInstance(Engine& engine)
    {
        if (q.get()) {
            throw "QtSingleton instance already set";
        }
        q.reset(new QtSingleton(engine));
    }
public:
    static QtSingleton& getInstance()
    {
        if (!q.get()) {
            throw "QtSingleton instance not set";
        }
        return *q;
    }
    Engine& getEngine()
    {
        return engine_;
    }
public:
    void Init()
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
    int Exec()
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
    Engine& engine_;
};


class Gui
{
public:
    Gui(Engine& engine)
    {
        QtSingleton::setInstance(engine);

        gtp.Register("gui", Gui::Run);
    }

    static void Run(Gtp::Io&);
};

#endif /* GO_GUI_H_ */
