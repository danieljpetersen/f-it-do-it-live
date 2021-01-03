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
#include "drawable_map.h"
#include "drawable_grid_lines.h"
#include "drawable_map_edges.h"
#include "last_plugin_core_state.h"
#include "audio.h"
#include "cities.h"

namespace IUEINW
{
    class IUEINW_Application : public fi::Application_Base, public fi::Plugin_Base, public fi::Singleton<IUEINW_Application>
    {
    public:
        int ProgramStateIndex_Core = -1;
        int ProgramStateIndex_MainMenu = -1;
        int ProgramStateIndex_LoadMenu = -1;
        int ProgramStateIndex_OptionsMenu = -1;
        int ProgramStateIndex_MapGenerationMenu = -1;
        int ProgramStateIndex_HallOfFameMenu = -1;
        int ProgramStateIndex_CreditsMenu = -1;

        // ---- bags of data / associated functions
        IUEINW_Map Map;
        IUEINW_Map_Drawables MapDrawables;
        IUEINW_Nations Nations;
        IUEINW_Color_Schemes ColorSchemes;
        IUEINW_Vision Vision;
        IUEINW_Cheats Cheats;
        IUEINW_Cities Cities;
        // Units
        // GovernmentTypes
        // Technologies

        // ---- Plugins related to Loading / Map Generation
        fi::Plugin_Loading_Start PluginLoadingStart;
        fi::Plugin_Loading_End PluginLoadingEnd;
        IUEINW_Plugin_Init_Color_Schemes PluginInitColorSchemes;
        IUEINW_Plugin_Generate_Map PluginGenerateMap;
        IUEINW_Plugin_Init_Cities PluginInitCities;
        IUEINW_Plugin_Init_Nations PluginInitNations;
        IUEINW_Plugin_Init_Vision PluginInitVision;
        IUEINW_Plugin_Init_Cheats PluginInitCheats;
        IUEINW_Plugin_Init_Map_Drawables PluginInitMapDrawables;

        // ---- Plugins related to fi::EVENT_UPDATE
        IUEINW_Plugin_Audio PluginAudio;
        IUEINW_Plugin_Controls PluginControls;
        IUEINW_Plugin_Mouse_Tile_Tracker PluginMouseTileTracker;
        IUEINW_Plugin_Last_Core_State PluginLastCoreState;

        // ---- Plugins related to fi::EVENT_DRAW / EVENT_BUILD_MAP_DRAWABLES
        IUEINW_Plugin_Build_Map_Drawables PluginBuildMapDrawables;
        IUEINW_Plugin_Draw_Map PluginDrawMap;
        IUEINW_Plugin_Draw_Cities PluginDrawCities;

        void setup() override
        {
            fi::getPlugins().defineProgramState(ProgramStateIndex_Core)
                    .withTick("core") // ticks defined in / auto loaded from settings json file

                    .withPlugin(&PluginLoadingStart, EVENT_MAP_GENERATION)
                    .withPlugin(&PluginInitColorSchemes, EVENT_MAP_GENERATION)
                    .withPlugin(&PluginGenerateMap, EVENT_MAP_GENERATION)
                    .withPlugin(&PluginInitCities, EVENT_MAP_GENERATION)
                    .withPlugin(&PluginInitNations, EVENT_MAP_GENERATION)
                    .withPlugin(&PluginInitVision, EVENT_MAP_GENERATION)
                    .withPlugin(&PluginInitCheats, EVENT_MAP_GENERATION)
                    .withPlugin(&PluginInitMapDrawables, EVENT_MAP_GENERATION)
                    .withPlugin(&PluginLoadingEnd, EVENT_MAP_GENERATION)

                    .withPlugin(this, fi::EVENT_UPDATE)
                    .withPlugin(&PluginMouseTileTracker, fi::EVENT_UPDATE)
                    .withPlugin(&PluginControls, fi::EVENT_UPDATE)
                    .withPlugin(&PluginAudio, fi::EVENT_UPDATE)
                    .withPlugin(&PluginLastCoreState, fi::EVENT_POST_UPDATE)

                    .withPlugin(&PluginBuildMapDrawables, EVENT_BUILD_MAP_DRAWABLES)

                    .withPlugin(&PluginDrawMap, fi::EVENT_DRAW)
                    .withPlugin(&PluginDrawCities, fi::EVENT_DRAW)
            ;

            fi::getPlugins().setProgramState(ProgramStateIndex_Core);
            fi::getPlugins().execute(EVENT_MAP_GENERATION);
            fi::getPlugins().execute(EVENT_BUILD_MAP_DRAWABLES);
        }


        void work(const int Event) override
        {
            //fi::log((int)Map.Tiles.LandTiles.size());
        }
    };

    IUEINW_Application &getApp();
}

#endif
