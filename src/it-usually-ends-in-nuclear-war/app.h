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
#include "user_interface.h"
#include "mouse_tile_tracker.h"
#include "drawable_map.h"
#include "drawable_grid_lines.h"
#include "drawable_map_edges.h"
#include "last_plugin_core_state.h"
#include "audio.h"
#include "cities.h"
#include "units.h"
#include "start_screen.h"
#include "ai.h"
#include "pathfinding.h"
#include "simulation.h"
#include "name_tags.h"

////////////////////////////////////////////////////////////

namespace IUEINW
{
    class IUEINW_Application : public fi::Application_Base, public fi::Plugin_Base, public fi::Singleton<IUEINW_Application>
    {
    public:
        int ProgramStateIndex_CoreScreen = -1;
        int ProgramStateIndex_StartScreen = -1;
        int ProgramStateIndex_LoadScreen = -1;
        int ProgramStateIndex_OptionsScreen = -1;
        int ProgramStateIndex_ConfigureMapGenerationScreen = -1;
        int ProgramStateIndex_HallOfFameScreen = -1;
        int ProgramStateIndex_CreditsScreen = -1;

        // ----
        IUEINW_Map Map;
        IUEINW_Map_Drawables MapDrawables;
        IUEINW_Nations Nations;
        IUEINW_Color_Schemes ColorSchemes;
        IUEINW_Vision Vision;
        IUEINW_Cheats Cheats;
        IUEINW_Cities Cities;
        IUEINW_Units Units;
        // GovernmentTypes
        // Technologies

        // ----
        IUEINW_Start_Screen PluginStartScreen;

        // ----
        fi::Plugin_Loading_Start PluginLoadingStart;
        fi::Plugin_Loading_End PluginLoadingEnd;
        IUEINW_Plugin_Init_Color_Schemes PluginInitColorSchemes;
        IUEINW_Plugin_Generate_Map PluginGenerateMap;
        IUEINW_Plugin_Init_Cities PluginInitCities;
        IUEINW_Plugin_Init_Units PluginInitUnits;
        IUEINW_Plugin_Init_Nations PluginInitNations;
        IUEINW_Plugin_Init_Cheats PluginInitCheats;
        IUEINW_Plugin_Init_Map_Drawables PluginInitMapDrawables;

        // ----
        IUEINW_Plugin_AI PluginAI;
        IUEINW_Plugin_Audio PluginAudio;
        IUEINW_Plugin_User_Interface PluginUI;
        IUEINW_Plugin_Simulation PluginSimulation;
		IUEINW_Plugin_Pathfinding PluginPathfinding;
        fi::Plugin_Camera_Zoom_GUI PluginWorldCanvasZoomGUI;
        IUEINW_Plugin_Mouse_Tile_Tracker PluginMouseTileTracker;
        IUEINW_Plugin_Last_Core_State PluginLastCoreState;

        // ----
        IUEINW_Plugin_Build_Map_Drawables PluginBuildMapDrawables;
        IUEINW_Plugin_Draw_Map PluginDrawMap;
        IUEINW_Plugin_Draw_Cities PluginDrawCities;
        IUEINW_Plugin_Draw_Units PluginDrawUnits;
        IUEINW_Plugin_Name_Tags PluginNameTags;

        void setup() override
        {
            fi::getPlugins().defineProgramState(ProgramStateIndex_StartScreen)
                .withPlugin(&PluginStartScreen, fi::EVENT_UPDATE)
            ;

            fi::getPlugins().defineProgramState(ProgramStateIndex_CoreScreen)
					.withTick("core") // ticks auto loaded from settings json file

					.withPlugin(&PluginLoadingStart, EVENT_MAP_GENERATION)
					.withPlugin(&PluginInitColorSchemes, EVENT_MAP_GENERATION)
					.withPlugin(&PluginGenerateMap, EVENT_MAP_GENERATION)
					.withPlugin(&PluginInitCities, EVENT_MAP_GENERATION)
					.withPlugin(&PluginInitUnits, EVENT_MAP_GENERATION)
					.withPlugin(&PluginInitNations, EVENT_MAP_GENERATION)
					.withPlugin(&PluginInitCheats, EVENT_MAP_GENERATION)
					.withPlugin(&PluginInitMapDrawables, EVENT_MAP_GENERATION)
					.withPlugin(&PluginLoadingEnd, EVENT_MAP_GENERATION)

					.withPlugin(this, fi::EVENT_UPDATE)
					.withPlugin(&PluginMouseTileTracker, fi::EVENT_UPDATE)
					.withPlugin(&PluginUI, fi::EVENT_UPDATE)
					.withPlugin(&PluginAI, fi::EVENT_UPDATE)
					.withPlugin(&PluginPathfinding, fi::EVENT_UPDATE)
					.withPlugin(&PluginAudio, fi::EVENT_UPDATE)
					.withPlugin(&PluginSimulation, fi::EVENT_UPDATE)
					.withPlugin(&PluginLastCoreState, fi::EVENT_UPDATE)

					.withPlugin(&PluginBuildMapDrawables, EVENT_BUILD_MAP_DRAWABLES)

					.withPlugin(&PluginDrawMap, fi::EVENT_DRAW)
					.withPlugin(&PluginDrawCities, fi::EVENT_DRAW)
					.withPlugin(&PluginUI, fi::EVENT_DRAW)
					.withPlugin(&PluginDrawUnits, fi::EVENT_DRAW)
					.withPlugin(&PluginNameTags, fi::EVENT_DRAW)
					.withPlugin(&PluginWorldCanvasZoomGUI, fi::EVENT_DRAW)
					;

			setProgramStateStartScreen();
        }

        // ----

        void work(const int Event) override
        {
        }

        // ----

        void setProgramStateStartScreen()
        {
            fi::getPlugins().setProgramState(ProgramStateIndex_StartScreen);
        }

        // ----

        void setProgramStateCoreScreen()
        {
            fi::getPlugins().setProgramState(ProgramStateIndex_CoreScreen);
            fi::getPlugins().delayedExecute(EVENT_MAP_GENERATION, fi::EVENT_STATE_CHANGE);
            fi::getPlugins().delayedExecute(EVENT_BUILD_MAP_DRAWABLES, fi::EVENT_STATE_CHANGE);
        }
    };

    IUEINW_Application &getApp();
}

#endif