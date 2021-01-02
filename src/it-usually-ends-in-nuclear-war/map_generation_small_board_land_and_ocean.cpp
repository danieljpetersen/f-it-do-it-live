#include "map_generation.h"
#include "map.h"

// yeah, this is an abomination, but just go with it, it works

void IUEINW::IUEINW_Plugin_Generate_Map::smallBoard_generateLandAndOcean()
{
	TileTypes.resize(SmallBoard.Grid.size());
	for (unsigned int i = 0; i < TileTypes.size(); i++)
		TileTypes[i] = MAP_GENERATOR_NOT_CHECKED;

	TilesToCheck.clear();
	int NumberOfLandTiles = 0, NumberOfWaterTiles = 0, ContinentStartingTile = 0, PercentChanceToMakeLand = 0, SizeOfContinentModifier = 2;

	const float LOW_RANGE_PERCENT_MAP_COVERED_IN_LAND = fi::getConfig()["game"]["map"]["percent-of-map-covered-in-land-low-range"].get<float>();
	const float HIGH_RANGE_PERCENT_MAP_COVERED_IN_LAND = fi::getConfig()["game"]["map"]["percent-of-map-covered-in-land-high-range"].get<float>();
	const float PERCENT_MAP_COVERED_IN_LAND = fi::getRandom().f(LOW_RANGE_PERCENT_MAP_COVERED_IN_LAND, HIGH_RANGE_PERCENT_MAP_COVERED_IN_LAND);

	const int NUMER_OF_DESIRED_LAND_TILES = fi::Math::getPercent(PERCENT_MAP_COVERED_IN_LAND, (float) SmallBoard.Grid.size());

	//every tile should be an ocean tile at this point.  make sure this is not changed in inittiles
	while (NumberOfLandTiles < NUMER_OF_DESIRED_LAND_TILES)
	{
		ContinentStartingTile = fi::getRandom().i(0, SmallBoard.Grid.size() - 1);

		TilesToCheck.push_front(ContinentStartingTile);
		TileTypes[ContinentStartingTile] = MAP_GENERATOR_MAKE_LAND;
		smallBoard_addSurroundingToTilesToLookAt(ContinentStartingTile);

		while (TilesToCheck.size() > 0)
		{
			int CheckTile = TilesToCheck.front();
			if (TileTypes[CheckTile] != MAP_GENERATOR_MAKE_LAND)
			{
				bool MakeTileLand = false;
				// i have no idea what the fuck this choice is for
				if (fi::getRandom().chance(50))
				{
					SizeOfContinentModifier = fi::getRandom().i(1, 200); //should be 1-60 seems to work well
					if (fi::getRandom().chance(5))
						SizeOfContinentModifier = 5;

					PercentChanceToMakeLand = 200 - (SmallBoard.Grid.getDistanceBetweenTiles(ContinentStartingTile, CheckTile) * SizeOfContinentModifier);
					if (PercentChanceToMakeLand < 0)
						PercentChanceToMakeLand = 10;

					if (fi::getRandom().i(0, 100) <= PercentChanceToMakeLand)
						MakeTileLand = true;
					else
						MakeTileLand = false;
				}
				else
				{
					SizeOfContinentModifier = fi::getRandom().i(1, 600); //100-600should be 1-60 seems to work well
					if (fi::getRandom().chance(5))
						SizeOfContinentModifier = 5;

					int Distance = SmallBoard.Grid.getDistanceBetweenTiles(ContinentStartingTile, CheckTile);

					PercentChanceToMakeLand = 600 - (Distance * SizeOfContinentModifier);
					if (PercentChanceToMakeLand < 0)
						PercentChanceToMakeLand = 5;

					if (fi::getRandom().i(0, 100) <= PercentChanceToMakeLand)
						MakeTileLand = true;
					else
						MakeTileLand = false;
				}
				if (MakeTileLand == true)
				{
					NumberOfLandTiles++;

					if (TileTypes[CheckTile] == MAP_GENERATOR_MAKE_WATER)
						NumberOfWaterTiles--;

					TileTypes[CheckTile] = MAP_GENERATOR_MAKE_LAND;
					smallBoard_addSurroundingToTilesToLookAt(CheckTile);
				}

				else // MakeTileLand == false
				{
					NumberOfWaterTiles++;
					TileTypes[CheckTile] = MAP_GENERATOR_MAKE_WATER;
				}

				TilesToCheck.pop_front();
			}

			else
				TilesToCheck.pop_front();

			if (NumberOfLandTiles >= NUMER_OF_DESIRED_LAND_TILES)
				break;
		}
	}

	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	//set small board
	for (int i = 0; i < SmallBoard.Grid.size(); i++)
	{
		int TileType = TileTypes[i];
		if (TileType == MAP_GENERATOR_MAKE_LAND)
			TileType = GRASS;
		else
			TileType = OCEAN;

		SmallBoard.Grid.CustomCellData[i].TileType = TileType;
	}
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Plugin_Generate_Map::smallBoard_addSurroundingToTilesToLookAt(int Index)
{
	const int MAKE_LAND = 1;

	int Neighbor;
	for (int i = TileToLeft; i < SmallBoard.Grid.Neighbors[Index].size(); i++)
	{
		Neighbor = SmallBoard.Grid.Neighbors[Index][i];
		if (Neighbor != -1)
		{
			if (TileTypes[Neighbor] != MAKE_LAND)
			{
				TilesToCheck.push_back(Neighbor);
			}
		}
	}
}