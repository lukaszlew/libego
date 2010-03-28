#ifndef GO_GUI_H_
#define GO_GUI_H_

#include "gtp.hpp"

class Engine;

class Gui
{
public:
    Gui(Engine& engine);

    static void Run(Gtp::Io& io);
};

#endif /* GO_GUI_H_ */
