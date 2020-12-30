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

            fi::getPlugins().defineProgramState(ProgramStateIndex_Core)
                    .withPlugin(this, true)
                    .withPlugin(&Controls, true)
                    .withPlugin(&Audio, true)
                    .withPlugin(&MouseTileTracker, true)
                    .withPlugin(&ColorSchemes, false)
                    .withPlugin(&Vision, false)
                    .withPlugin(&Map, false)
                    //.withPlugin(&Provinces, true)
                    .withPlugin(&Nations, true)
                    //.withPlugin(&Units, true)
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

        void onUpdate() override
        {
        }

        void onDraw() override
        {
        }
    };

    IUEINW_Application &getApp();
}

#endif
