#include <vector>
#include "../common/engine.h"
#include "vision.h"
#include "nations.h"
#include "cheats.h"
#include "map.h"
#include "drawable_map.h"

void IUEINW::IUEINW_Vision::init()
{
	VisionCount.clear();
	LastTickSeen.clear();
	for (int TileIndex = 0; TileIndex < getTiles().Grid.size(); TileIndex++)
	{
		LastTickSeen.push_back(-1);
		VisionCount.push_back(-1);
	}

	EdgeTilesDiscovered.clear();
	EdgeTilesDiscovered.resize(4);
	EdgeTilesDiscovered[0].clear();
	EdgeTilesDiscovered[1].clear();
	EdgeTilesDiscovered[2].clear();
	EdgeTilesDiscovered[3].clear();
}

////////////////////////////////////////////////////////////
// for cheat mode changing human player; will
void IUEINW::IUEINW_Vision::recalculateVision()
{
	fi::log("todo recalculate vision");

	init();
	// todo here loop through all owned land and increment vision
	// todo here loop through all units and inncrement vision for all tiles within that units vision range
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Vision::revealTile(int TileIndex)
{
	incrementVision(TileIndex);
	decrementVision(TileIndex);
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Vision::incrementArea(int CenterTileIndex, int HalfAreaSize)
{
	int CoTSize;
	std::vector<int> *CoT = getTiles().getArea(CenterTileIndex, HalfAreaSize, Tile_Type_Grouping::ALL_TILE_TYPES, CoTSize);
	for (unsigned int i = 0; i < CoTSize; i++)
	{
		int TileIndex = CoT->at(i);
		incrementVision(TileIndex);
	}
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Vision::decrementArea(int CenterTileIndex, int HalfAreaSize)
{
	int CoTSize;
	std::vector<int> *CoT = getTiles().getArea(CenterTileIndex, HalfAreaSize, Tile_Type_Grouping::ALL_TILE_TYPES, CoTSize);
	for (unsigned int i = 0; i < CoTSize; i++)
	{
		int TileIndex = CoT->at(i);
		decrementVision(TileIndex);
	}
}


////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Vision::incrementVision(int TileIndex)
{
	VisionCount[TileIndex] += 1;
	LastTickSeen[TileIndex] = std::max(1, getCoreTick()->getTickCount());

	// values are flagged as -1 to indicate not ever explored.
	if (VisionCount[TileIndex] == 0)
	{
		VisionCount[TileIndex] += 1;

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
				EdgeTilesDiscovered[BOTTOMLEFT_EDGE].push_back(TileIndex);
				break;
			}

			case BOTTOMRIGHT_EDGE:
			{
				EdgeTilesDiscovered[BOTTOMRIGHT_EDGE].push_back(TileIndex);
				break;
			}

			case TOPLEFT_EDGE:
			{
				EdgeTilesDiscovered[TOPLEFT_EDGE].push_back(TileIndex);
				break;
			}

			case TOPRIGHT_EDGE:
			{
				EdgeTilesDiscovered[TOPRIGHT_EDGE].push_back(TileIndex);
				break;
			}

			case RIGHTCORNER_EDGE:
			{
				EdgeTilesDiscovered[TOPRIGHT_EDGE].push_back(TileIndex);
				EdgeTilesDiscovered[BOTTOMRIGHT_EDGE].push_back(TileIndex);
				break;
			}

			case TOPCORNER_EDGE:
			{
				EdgeTilesDiscovered[TOPLEFT_EDGE].push_back(TileIndex);
				EdgeTilesDiscovered[TOPRIGHT_EDGE].push_back(TileIndex);
				break;
			}

			case BOTTOMCORNER_EDGE:
			{
				EdgeTilesDiscovered[BOTTOMLEFT_EDGE].push_back(TileIndex);
				EdgeTilesDiscovered[BOTTOMRIGHT_EDGE].push_back(TileIndex);
				break;
			}

			case LEFTCORNER_EDGE:
			{
				EdgeTilesDiscovered[TOPLEFT_EDGE].push_back(TileIndex);
				EdgeTilesDiscovered[BOTTOMLEFT_EDGE].push_back(TileIndex);
				break;
			}
		}

		getMapDrawables().setTileVertexColors(TileIndex, true);
	}
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Vision::decrementVision(int TileIndex)
{
	VisionCount[TileIndex] -= 1;
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Vision::clearVision(int TileIndex)
{
	VisionCount[TileIndex] = -1;
	LastTickSeen[TileIndex] = -1;
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasDiscoveredBottomLeftEdge()
{
	if (getCheats().isRevealAllTerrainEnabled())
		return true;

	return hasDiscoveredBottomLeftEdge_ignoreCheats();
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasDiscoveredBottomRightEdge()
{
	if (getCheats().isRevealAllTerrainEnabled())
		return true;

	return hasDiscoveredBottomRightEdge_ignoreCheats();
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasDiscoveredTopLeftEdge()
{
	if (getCheats().isRevealAllTerrainEnabled())
		return true;

	return hasDiscoveredTopLeftEdge_ignoreCheats();
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasDiscoveredTopRightEdge()
{
	if (getCheats().isRevealAllTerrainEnabled())
		return true;

	return hasDiscoveredTopRightEdge_ignoreCheats();
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasDiscoveredAllEdges()
{
	if (hasDiscoveredBottomLeftEdge())
	{
		if (hasDiscoveredBottomRightEdge())
		{
			if (hasDiscoveredTopLeftEdge())
			{
				if (hasDiscoveredTopRightEdge())
				{
					return true;
				}
			}
		}
	}
	return false;
}

////////////////////////////////////////////////////////////

int IUEINW::IUEINW_Vision::lastTickSeen_ignoreCheatMode(int TileIndex)
{
	return LastTickSeen[TileIndex];
}

////////////////////////////////////////////////////////////

int IUEINW::IUEINW_Vision::lastTickSeen(int TileIndex)
{
	if (getCheats().isRevealAllTerrainEnabled())
	{
		return getCoreTick()->getTickCount();
	}

	if (getCheats().isNoFogOfWarEnabled())
	{
		if (hasExplored_IgnoreCheatMode(TileIndex))
		{
			return getCoreTick()->getTickCount();
		}
	}

	if (this->VisionCount[TileIndex] > 0)
	{
		return getCoreTick()->getTickCount();
	}

	return LastTickSeen[TileIndex];
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasExplored(int TileIndex)
{
	if (getCheats().isRevealAllTerrainEnabled())
		return true;

	return hasExplored_IgnoreCheatMode(TileIndex);
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasExplored_IgnoreCheatMode(int TileIndex)
{
	if (LastTickSeen[TileIndex] >= 0)
	{
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasVision(int TileIndex)
{
	if (getCheats().isNoFogOfWarEnabled())
	{
		if (hasExplored_IgnoreCheatMode(TileIndex))
		{
			return true;
		}

		if (getCheats().isRevealAllTerrainEnabled())
		{
			return true;
		}
	}

	return hasVision_IgnoreCheatMode(TileIndex);
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasVision_IgnoreCheatMode(int TileIndex)
{
	return VisionCount[TileIndex] > 0;
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasDiscoveredBottomLeftEdge_ignoreCheats()
{
	return (bool)EdgeTilesDiscovered[BOTTOMLEFT_EDGE].empty();
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasDiscoveredBottomRightEdge_ignoreCheats()
{
	return (bool)EdgeTilesDiscovered[BOTTOMRIGHT_EDGE].empty();
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasDiscoveredTopLeftEdge_ignoreCheats()
{
	return (bool)EdgeTilesDiscovered[TOPLEFT_EDGE].empty();
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Vision::hasDiscoveredTopRightEdge_ignoreCheats()
{
	return (bool)EdgeTilesDiscovered[TOPRIGHT_EDGE].empty();
}