#include "iueinw.h"
#include "vision.h"

int IUEINW::CITY_RING_SIZE = 0;

////////////////////////////////////////////////////////////

IUEINW::IUEINW_Application &IUEINW::getApp()
{
    return IUEINW::IUEINW_Application::instance();
}

////////////////////////////////////////////////////////////

IUEINW::IUEINW_Map &IUEINW::getMap()
{
    return getApp().Map;
}

////////////////////////////////////////////////////////////

IUEINW::IUEINW_Tiles &IUEINW::getTiles()
{
    return getApp().Map.Tiles;
}

////////////////////////////////////////////////////////////

fi::Grid_1D_Custom_Data<IUEINW::IUEINW_Tile> &IUEINW::getGrid()
{
    return getApp().Map.Tiles.Grid;
}

////////////////////////////////////////////////////////////

IUEINW::IUEINW_Color_Schemes &IUEINW::getColorSchemes()
{
    return getApp().ColorSchemes;
}

////////////////////////////////////////////////////////////

IUEINW::IUEINW_Nations &IUEINW::getNations()
{
    return getApp().Nations;
}

////////////////////////////////////////////////////////////

IUEINW::IUEINW_Vision &IUEINW::getVision()
{
    return getApp().Vision;
}

////////////////////////////////////////////////////////////

IUEINW::IUEINW_Cheats &IUEINW::getCheats()
{
    return getApp().Cheats;
}

////////////////////////////////////////////////////////////

int IUEINW::getProgramState_Core()
{
    return getApp().ProgramStateIndex_Core;
}

////////////////////////////////////////////////////////////

IUEINW::IUEINW_Plugin_Mouse_Tile_Tracker &IUEINW::getMouseTileTracker()
{
    return getApp().PluginMouseTileTracker;
}

////////////////////////////////////////////////////////////

IUEINW::IUEINW_Map_Drawables &IUEINW::getMapDrawables()
{
    return getApp().MapDrawables;
}

////////////////////////////////////////////////////////////

IUEINW::IUEINW_Cities &IUEINW::getCities()
{
    return getApp().Cities;
}

