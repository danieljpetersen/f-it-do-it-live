#include <vector>
#include "../common/engine.h"
#include "vision.h"
#include "nations.h"
#include "cheats.h"
#include "map.h"
#include "drawable_map.h"

void IUEINW::IUEINW_Vision::init(int NumberOfNations)
{
	VisionCount.clear();
	LastTickSeen.clear();
	for (int TileIndex = 0; TileIndex < getTiles().Grid.size(); TileIndex++)
	{
		LastTickSeen.push_back(std::vector<int>());
		VisionCount.push_back(std::vector<int>());
		for (int NationIndex = 0; NationIndex < NumberOfNations; NationIndex++)
		{
			VisionCount[TileIndex].push_back(-1); // very important to be -1 by default and not 0. see comment in incrementVision
			LastTickSeen[TileIndex].push_back(-1);
		}
	}
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Vision::revealTile(int TileIndex, int NationIndex)
{
	incrementVision(TileIndex, NationIndex);
	decrementVision(TileIndex, NationIndex);
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Vision::incrementArea(int CenterTileIndex, int NationIndex, int HalfAreaSize)
{
	int CoTSize;
	std::vector<int> *CoT = getTiles().getArea(CenterTileIndex, HalfAreaSize, Tile_Type_Grouping::ALL_TILE_TYPES, CoTSize);
	for (unsigned int i = 0; i < CoTSize; i++)
	{
		int TileIndex = CoT->at(i);
		incrementVision(TileIndex, NationIndex);
	}
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Vision::decrementArea(int CenterTileIndex, int NationIndex, int HalfAreaSize)
{
	int CoTSize;
	std::vector<int> *CoT = getTiles().getArea(CenterTileIndex, HalfAreaSize, Tile_Type_Grouping::ALL_TILE_TYPES, CoTSize);
	for (unsigned int i = 0; i < CoTSize; i++)
	{
		int TileIndex = CoT->at(i);
		decrementVision(TileIndex, NationIndex);
	}
}


////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Vision::incrementVision(int TileIndex, int NationIndex)
{
	VisionCount[TileIndex][NationIndex] += 1;
	LastTickSeen[TileIndex][NationIndex] = getCoreTick()->getTickCount();

	// By Default, values are flagged as -1 to indicate not ever explored.
	// If we increment and the value is 0, we know this is the first time we've visited this tile
	// todo what the fuck this doesn't explain what is wrong with just checking for 1 here
	if (VisionCount[TileIndex][NationIndex] == 0)
	{
		VisionCount[TileIndex][NationIndex] += 1;

		switch (getTiles().Grid.CommonCellData[TileIndex].Edge)
		{
			case NO_EDGE:
			{
				break;
			}

			default:
			{
				break;
			}

			case BOTTOMLEFT_EDGE:
			{
				getNations().Nations[NationIndex].EdgeTilesDiscovered[BOTTOMLEFT_EDGE].push_back(TileIndex);
				break;
			}

			case BOTTOMRIGHT_EDGE:
			{
				getNations().Nations[NationIndex].EdgeTilesDiscovered[BOTTOMRIGHT_EDGE].push_back(TileIndex);
				break;
			}

			case TOPLEFT_EDGE:
			{
				getNations().Nations[NationIndex].EdgeTilesDiscovered[TOPLEFT_EDGE].push_back(TileIndex);
				break;
			}

			case TOPRIGHT_EDGE:
			{
				getNations().Nations[NationIndex].EdgeTilesDiscovered[TOPRIGHT_EDGE].push_back(TileIndex);
				break;
			}

			case RIGHTCORNER_EDGE:
			{
				getNations().Nations[NationIndex].EdgeTilesDiscovered[TOPRIGHT_EDGE].push_back(TileIndex);
				getNations().Nations[NationIndex].EdgeTilesDiscovered[BOTTOMRIGHT_EDGE].push_back(TileIndex);
				break;
			}

			case TOPCORNER_EDGE:
			{
				getNations().Nations[NationIndex].EdgeTilesDiscovered[TOPLEFT_EDGE].push_back(TileIndex);
				getNations().Nations[NationIndex].EdgeTilesDiscovered[TOPRIGHT_EDGE].push_back(TileIndex);
				break;
			}

			case BOTTOMCORNER_EDGE:
			{
				getNations().Nations[NationIndex].EdgeTilesDiscovered[BOTTOMLEFT_EDGE].push_back(TileIndex);
				getNations().Nations[NationIndex].EdgeTilesDiscovered[BOTTOMRIGHT_EDGE].push_back(TileIndex);
				break;
			}

			case LEFTCORNER_EDGE:
			{
				getNations().Nations[NationIndex].EdgeTilesDiscovered[TOPLEFT_EDGE].push_back(TileIndex);
				getNations().Nations[NationIndex].EdgeTilesDiscovered[BOTTOMLEFT_EDGE].push_back(TileIndex);
				break;
			}
		}

		if (NationIndex == getNations().HumanNationIndex)
		{
			getMapDrawables().setTileVertexColors(TileIndex, true);
		}

		//fi::flagUpdate();
	}
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Vision::decrementVision(int TileIndex, int NationIndex)
{
	VisionCount[TileIndex][NationIndex] -= 1;
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Vision::clearVision(int TileIndex, int NationIndex)
{
	VisionCount[TileIndex][NationIndex] = -1;
	LastTickSeen[TileIndex][NationIndex] = -1;
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Vision::clearVision_AllNations(int TileIndex)
{
	for (int NationIndex = 0; NationIndex < getNations().size(); NationIndex++)
	{
		clearVision(TileIndex, NationIndex);
	}
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasHumanNationDiscoveredBottomLeftEdge()
{
	if (getCheats().isRevealAllTerrainEnabled())
		return true;

	return getNations().Human->discoveredBottomLeftEdge();
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasHumanNationDiscoveredBottomRightEdge()
{
	if (getCheats().isRevealAllTerrainEnabled())
		return true;

	return getNations().Human->discoveredBottomRightEdge();
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasHumanNationDiscoveredTopLeftEdge()
{
	if (getCheats().isRevealAllTerrainEnabled())
		return true;

	return getNations().Human->discoveredTopLeftEdge();
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasHumanNationDiscoveredTopRightEdge()
{
	if (getCheats().isRevealAllTerrainEnabled())
		return true;

	return getNations().Human->discoveredTopRightEdge();
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasHumanNationDiscoveredAllEdges()
{
	if (hasHumanNationDiscoveredBottomLeftEdge())
	{
		if (hasHumanNationDiscoveredBottomRightEdge())
		{
			if (hasHumanNationDiscoveredTopLeftEdge())
			{
				if (hasHumanNationDiscoveredTopRightEdge())
				{
					return true;
				}
			}
		}
	}
	return false;
}

////////////////////////////////////////////////////////////

int IUEINW::IUEINW_Vision::lastTickSeen_ignoreCheatMode(int TileIndex, int NationIndex)
{
	return LastTickSeen[TileIndex][NationIndex];
}

////////////////////////////////////////////////////////////

int IUEINW::IUEINW_Vision::lastTickSeen(int TileIndex, int NationIndex)
{
	if (getCheats().isNoFogOfWarEnabled())
	{
		if (getCheats().isRevealAllTerrainEnabled())
		{
			return getCoreTick()->getTickCount();
		}
		else
		{
			if (hasNationExplored_IgnoreCheatMode(NationIndex, TileIndex))
			{
				if (this->VisionCount[TileIndex][NationIndex] > 0)
				{
					return getCoreTick()->getTickCount();
				}

				return LastTickSeen[TileIndex][NationIndex];
			}
		}
	}

	if (this->VisionCount[TileIndex][NationIndex] > 0)
	{
		return getCoreTick()->getTickCount();
	}

	return LastTickSeen[TileIndex][NationIndex];
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasNationExplored(int NationIndex, int TileIndex)
{
	// cheat mode only applies to human vision
	bool CheatModeReveal = false;
	if (NationIndex == getNations().HumanNationIndex)
	{
		CheatModeReveal = getCheats().isRevealAllTerrainEnabled();
	}

	if (NationIndex != -1)
	{
		if ((VisionCount[TileIndex][NationIndex] >= 0) || (CheatModeReveal))
		{
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasNationExplored_IgnoreCheatMode(int NationIndex, int TileIndex)
{
	if (VisionCount[TileIndex][NationIndex] >= 0)
	{
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasHumanNationExplored(int TileIndex)
{
	return hasNationExplored(getNations().HumanNationIndex, TileIndex);
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasHumanNationExplored_IgnoreCheatMode(int TileIndex)
{
	return hasNationExplored_IgnoreCheatMode(getNations().HumanNationIndex, TileIndex);
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasVision(int TileIndex)
{
	if (getCheats().isNoFogOfWarEnabled())
	{
		if (getCheats().isRevealAllTerrainEnabled())
		{
			return true;
		}
		else
		{
			return hasHumanNationExplored(TileIndex);
		}
	}

	return hasVision_IgnoreCheatMode(TileIndex);
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasVision_IgnoreCheatMode(int TileIndex)
{
	return VisionCount[TileIndex][getNations().HumanNationIndex] > 0;
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::canHumanSeeCity(int TileIndex, int TickCityCreated)
{
	return (getCheats().isRevealAllTerrainEnabled()) || (lastTickSeen(TileIndex, getNations().HumanNationIndex) > TickCityCreated);
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::canAnyoneSee(int TileIndex)
{
	for (int i = 0; i < getNations().size(); i++)
	{
		if (canNationSee(TileIndex, i))
		{
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::canNationSee(int TileIndex, int NationIndex)
{
	return this->VisionCount[NationIndex][TileIndex] > 0;
}
