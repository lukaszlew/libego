#ifndef GO_GUI_H_
#define GO_GUI_H_

#include "game_inc.h"

class Gui
{
public:
    Gui(Engine& engine);

    static void Run(Gtp::Io&);
};

#endif /* GO_GUI_H_ */
