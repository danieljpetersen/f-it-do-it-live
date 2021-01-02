#include "cheats.h"
#include "../common/engine.h"
#include "map.h"
#include "drawable_map.h"

void IUEINW::IUEINW_Plugin_Init_Cheats::work(const int Event)
{
    getCheats().reset();
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Cheats::reset()
{
    RevealAllTerrain = false;
    NoFogOfWar = false;
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Cheats::isNoFogOfWarEnabled()
{
	return NoFogOfWar;
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Cheats::isRevealAllTerrainEnabled()
{
	return RevealAllTerrain;
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Cheats::toggleFow()
{
	NoFogOfWar = !NoFogOfWar;
    fi::getPlugins().delayedExecute(EVENT_BUILD_MAP_DRAWABLES);
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Cheats::toggleRevealTerrain()
{
	RevealAllTerrain = !RevealAllTerrain;
    fi::getPlugins().delayedExecute(EVENT_BUILD_MAP_DRAWABLES);
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Cheats::setRevealTerrain(bool Value)
{
	RevealAllTerrain = Value;
    fi::getPlugins().delayedExecute(EVENT_BUILD_MAP_DRAWABLES);
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Cheats::setNoFogOfWar(bool Value)
{
	NoFogOfWar = Value;
    fi::getPlugins().delayedExecute(EVENT_BUILD_MAP_DRAWABLES);
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Cheats::activate()
{
	Active = true;
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Cheats::deactivate()
{
	Active = false;
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Cheats::isActive()
{
	return Active;
}

////////////////////////////////////////////////////////////

