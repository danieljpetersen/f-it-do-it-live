#ifndef FI_IUEINW_GLOBALS_H
#define FI_IUEINW_GLOBALS_H

#include "../common/engine.h"

namespace IUEINW
{
    enum Event
    {
        EVENT_MAP_GENERATION = fi::FI_EVENT_COUNT,
        EVENT_BUILD_MAP_DRAWABLES,
    };

    extern int CITY_RING_SIZE;

    int getProgramState_Core();
}

#endif