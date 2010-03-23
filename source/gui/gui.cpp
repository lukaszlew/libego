#include <QtGui>
#include <QApplication>
#include <boost/shared_ptr.hpp>
#include "gui.h"
#include "engine.hpp"
#include "gogui.h"


class QtSingleton
{
friend class Gui;
private:
    QtSingleton(Engine& engine) :
        app_(NULL),
        engine_(engine)
    {
        Q_INIT_RESOURCE(images);

        argc_ = 1;
        argv_ = (char**)malloc(sizeof(char*));
        argv_[0] = (char*)malloc(6*sizeof(char));
        strncpy(argv_[0], "GoGui\0", 6);
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
    ~QtSingleton()
    {
        free(argv_[0]);
        free(argv_);
    }

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

        getInstance().app_ = new QApplication(argc_, argv_);

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
    int argc_;
    char **argv_;
};

boost::shared_ptr<QtSingleton> QtSingleton::q = boost::shared_ptr<QtSingleton>();


Gui::Gui(Engine& engine)
{
    QtSingleton::setInstance(engine);

    gtp.Register("gui", Gui::Run);
}



void Gui::Run(Gtp::Io&)
{
    std::cout << "Gui::Run" << std::endl;
    QtSingleton::getInstance().Init();

    GoGui gogui(QtSingleton::getInstance().getEngine());
    gogui.show();

    int ret = QtSingleton::getInstance().Exec();

    std::cout << "Gui::Exit with " << ret << std::endl;
}

