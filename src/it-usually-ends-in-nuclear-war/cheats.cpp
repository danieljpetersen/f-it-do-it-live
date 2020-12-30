#include "cheats.h"
#include "../common/engine.h"
#include "map.h"

void IUEINW::IUEINW_Cheats::onProgramStart()
{
	RevealAllTerrain = false;
	NoFogOfWar = false;
}

//==================================

bool IUEINW::IUEINW_Cheats::isNoFogOfWarEnabled()
{
	return NoFogOfWar;
}

//==================================

bool IUEINW::IUEINW_Cheats::isRevealAllTerrainEnabled()
{
	return RevealAllTerrain;
}

//==================================

void IUEINW::IUEINW_Cheats::toggleFow()
{
	NoFogOfWar = !NoFogOfWar;
	fi::flagUpdate();
	getMap().setTileVertexColors();
}

//==================================

void IUEINW::IUEINW_Cheats::toggleRevealTerrain()
{
	RevealAllTerrain = !RevealAllTerrain;
	fi::flagUpdate();
	getMap().setTileVertexColors();
}

//==================================

void IUEINW::IUEINW_Cheats::setRevealTerrain(bool Value)
{
	RevealAllTerrain = Value;
	getMap().setTileVertexColors();
    fi::flagUpdate();
}

//==================================

void IUEINW::IUEINW_Cheats::setNoFogOfWar(bool Value)
{
	NoFogOfWar = Value;
	getMap().setTileVertexColors();
	fi::flagUpdate();
}

//==================================

void IUEINW::IUEINW_Cheats::activate()
{
	Active = true;
}

//==================================

void IUEINW::IUEINW_Cheats::deactivate()
{
	Active = false;
}

//==================================

bool IUEINW::IUEINW_Cheats::isActive()
{
	return Active;
}

//==================================

