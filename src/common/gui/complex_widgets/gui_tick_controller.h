#ifndef FI_GUI_TICK_CONTROLLER_H
#define FI_GUI_TICK_CONTROLLER_H

#include "../../tick.h"

namespace fi
{
    class Tick_Controller
    {
    public:
        int SelectedTickSpeed = 0;
        int TickSpeedPriorToPause = 4;
        bool ShowingProgressBarForTickCounter = false;

        void update(fi::Tick *Tick);
    };
}

#endif