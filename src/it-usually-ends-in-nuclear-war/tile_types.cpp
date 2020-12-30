#include "tile_types.h"
#include "../common/engine.h"
#include "map.h"

void IUEINW::IUEINW_Tile_Type_Properties::initFromJSON()
{
	DefaultProductionByTileType.resize(NUMBER_OF_TILE_TYPES);
	DefaultFoodByTileType.resize(NUMBER_OF_TILE_TYPES);

	auto TileOutputs = fi::getConfig()["gameplay"]["tile-outputs"];
	for (auto it = TileOutputs.begin(); it != TileOutputs.end(); ++it)
	{
		std::string Name = it.key();
		int Food = (*it)["food"].get<int>();
		int Production = (*it)["production"].get<int>();

		int TileType = getMap().TileTypeProperties.convertStringToTileType(Name);
		DefaultFoodByTileType[TileType] = Food;
		DefaultProductionByTileType[TileType] = Production;
	}
}

//==================================

void IUEINW::IUEINW_Tile_Type_Properties::rankTiles()
{
	TileRanks.clear();
	TileRanks.resize((unsigned long)getTiles().Grid.NumberOfCells);

	SortedTileRanks_PerContinent.clear();
	SortedTileRanks_PerContinent.resize(getTiles().Continents.size());

	int ContinentIndex;
	for (int i = 0; i < getTiles().Grid.NumberOfCells; i++)
	{
		TileRanks[i] = rankTile(i);

		ContinentIndex = getTiles().Grid.CustomCellData[i].ContinentIndex;
		if (ContinentIndex != -1)
		{
			SortedTileRanks_PerContinent[ContinentIndex].push_back(TileRanks[i]);
		}
	}

	for (int ContinentIndex = 0; ContinentIndex < SortedTileRanks_PerContinent.size(); ContinentIndex++)
	{
		// sort high to low
		std::sort(SortedTileRanks_PerContinent[ContinentIndex].begin(), SortedTileRanks_PerContinent[ContinentIndex].end(), fi::sort_high_to_low());
	}
}

//==================================

std::vector<int> IUEINW::IUEINW_Tile_Type_Properties::getSpawnTiles(int &NumberOfNations)
{
	std::vector<int> SpawnRanks, SpawnTiles;

	int LowestRankScore=-1, LowestRankIndex=-1;
	for (int i = 0; i < 1000; i++)
	{
		int PossibleSpawnTile = getTiles().LandTiles[fi::getRandom().i(0, getTiles().LandTiles.size()-1)];

		if (getTiles().Grid.CustomCellData[PossibleSpawnTile].isWaterTile() != true)
		{
			bool AddTile = true;
			int CoTSize;
			std::vector<int> *CoT = getTiles().getArea(PossibleSpawnTile, INITIAL_SPAWN_AREA_SEARCH_SIZE, Tile_Type_Grouping::ALL_TILE_TYPES, CoTSize);
			for (int j = 0; j < CoTSize; j++)
			{
				int Index = CoT->at(j);
				if (fi::vectorContains(&SpawnTiles, Index))
				{
					AddTile = false;
					break;
				}
			}

			if (AddTile)
			{
				int SpawnRank = 0;
				for (int j = 0; j < CoTSize; j++)
				{
					SpawnRank += rankTile(CoT->at(j));
				}

				if (CoTSize < CITY_RING_SIZE)
				{
					SpawnRank -= 10 * (CITY_RING_SIZE - CoTSize);
				}

				if (SpawnRank > 30)
				{
					SpawnTiles.push_back(PossibleSpawnTile);
					SpawnRanks.push_back(SpawnRank);
				}
			}
		}
	}

	if (NumberOfNations > SpawnRanks.size())
	{
		NumberOfNations = SpawnRanks.size();
	}

	// I know this is terrible, but honestly, it's fine.
	std::vector<SpawnTileRank> Sorted;
	for (int i = 0; i < SpawnRanks.size(); i++)
	{
		SpawnTileRank Rank;
		Rank.TileIndex = SpawnTiles[i];
		Rank.Rank = SpawnRanks[i];
		Sorted.push_back(Rank);
	}
	std::sort(Sorted.begin(), Sorted.end(), SortTilesByRank());

	SpawnTiles.clear();
	for (int i = 0; i < Sorted.size(); i++)
	{
		SpawnTiles.push_back(Sorted[i].TileIndex);
	}

	//----

	// prune tiles too close
	{
		std::vector<int> RemovedTiles;
		for (int i = 0; i < SpawnTiles.size(); i++)
		{
			for (int j = i+1; j < SpawnTiles.size(); j++)
			{
				int Distance = getTiles().Grid.getDistanceBetweenTiles(SpawnTiles[j], SpawnTiles[i]);
				if (Distance <= 19)
				{
					RemovedTiles.push_back(SpawnTiles[j]);
					SpawnTiles.erase(SpawnTiles.begin() + j);
					j--;
				}
			}
		}

		if (SpawnTiles.size() < NumberOfNations)
		{
			int Difference = NumberOfNations - SpawnTiles.size();
			for (int i = 0; i < Difference; i++)
			{
				SpawnTiles.push_back(RemovedTiles[i]);
			}
		}
	}

	std::vector<int> ReturnValue;
	for (int i = 0; i < NumberOfNations; i++)
	{
		ReturnValue.push_back(SpawnTiles[i]);
	}

	return ReturnValue;
}

//==================================

int IUEINW::IUEINW_Tile_Type_Properties::rankTile(int TileIndex)
{
	// todo -- need another function similar to this which takes into account neighboring tiles for city search
	int Rank = getTiles().Grid.CustomCellData[TileIndex].ProductionOutputBase + getTiles().Grid.CustomCellData[TileIndex].FoodOutputBase;
	return Rank;
}

//==================================

int IUEINW::IUEINW_Tile_Type_Properties::convertStringToTileType(std::string TileTypeString)
{
	std::string downcased = fi::stringDowncase(TileTypeString);
	if ((downcased == "grassland") || (downcased == "grass"))
	{
		return GRASS;
	}
	else if (downcased == "ice")
	{
		return ICE;
	}
	else if (downcased == "forest")
	{
		return FOREST;
	}
	else if (downcased == "desert")
	{
		return DESERT;
	}
	else if (downcased == "ocean")
	{
		return OCEAN;
	}
	else if (downcased == "coastal")
	{
		return COASTAL_OCEAN;
	}
	else if (downcased == "plains")
	{
		return PLAINS;
	}

	return -1;
}