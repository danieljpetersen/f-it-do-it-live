#ifndef IUEINW_APP_H
#define IUEINW_APP_H

#include "../common/engine.h"
#include "globals.h"
#include "map_generation.h"
#include "map.h"
#include "color_schemes.h"
#include "nations.h"
#include "vision.h"
#include "cheats.h"
#include "controls.h"
#include "mouse_tile_tracker.h"
#include "drawable_grid_lines.h"
#include "drawable_map_edges.h"
#include "last_plugin_core_state.h"
#include "audio.h"

namespace IUEINW
{
    class IUEINW_Application : public fi::Application_Base, public fi::Plugin_Base, public fi::Singleton<IUEINW_Application>
    {
    public:
        int ProgramStateIndex_Core = -1;
        int ProgramStateIndex_MapGeneration = -1;

        IUEINW_Map Map;
        IUEINW_Map_Generator MapGenerator;
        IUEINW_Color_Schemes ColorSchemes;
        IUEINW_Audio Audio;
        IUEINW_Nations Nations;
        IUEINW_Vision Vision;
        IUEINW_Cheats Cheats;
        IUEINW_Controls Controls;
        IUEINW_Mouse_Tile_Tracker MouseTileTracker;
        IUEINW_Drawable_Grid_Lines_Around_Mouse DrawableGridLinesAroundMouse;
        IUEINW_Drawable_Grid_Lines_Always_On DrawableGridLinesAlwaysOn;
        IUEINW_Drawable_Map_Land_Ocean_Border DrawableMapLandOceanBorder;
        IUEINW_Drawable_Map_Edges DrawableMapEdges;
        IUEINW_Last_Plugin_Core_State LastPluginCoreState;

        void setup() override
        {
            fi::getPlugins().defineProgramState(ProgramStateIndex_MapGeneration)
                    .withPlugin(&MapGenerator, false);
            ;

            /*

             const int EVENT_ID_PROGRAM_START
             const int EVENT_ID_ON_RESIZE
             const int EVENT_ID_PRE_UPDATE
             const int EVENT_ID_ON_UPDATE
             const int EVENT_ID_POST_UPDATE
             const int EVENT_ID_ON_DRAW
             const int EVENT_ID_ON_QUIT
             const int EVENT_CORE_TICK

             Program_State_Builder &withPlugin(*Plugin, EventID)

             .withPlugin(Controls, EVENT_ON_UPDATE)
             .withPlugin(*plugin, EVENT_MAP_GENERATION);

             vector<Plugin*> Plugins;
             unordered_map<vector<int>> Plugins;
             Plugins.resize(NUMBER_OF_DEFAULT_EVENTS);


             map generation
             unit created
             city created

             defineProgram()
             .withPlugin(Controls, EVENT_CORE_TICK)



             */


            fi::getPlugins().defineProgramState(ProgramStateIndex_Core)
                    .withPlugin(this, true)
                    .withPlugin(&Controls, true)
                    .withPlugin(&Audio, true)
                    .withPlugin(&MouseTileTracker, true)
                    .withPlugin(&ColorSchemes, false)
                    .withPlugin(&Vision, false)
                    .withPlugin(&Map, false)
                    .withPlugin(&Nations, true)
                    .withPlugin(&DrawableGridLinesAlwaysOn, true)
                    .withPlugin(&DrawableGridLinesAroundMouse, true)
                    .withPlugin(&DrawableMapEdges, false)
                    .withPlugin(&DrawableMapLandOceanBorder, false)
                    .withPlugin(&Cheats, true)
                    .withPlugin(&LastPluginCoreState, true)
                    .withTick("core")
            ;

            fi::getPlugins().push(ProgramStateIndex_MapGeneration);
        }
    };

    IUEINW_Application &getApp();
}

#endif
