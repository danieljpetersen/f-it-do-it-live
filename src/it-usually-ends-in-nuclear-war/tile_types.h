#ifndef IT_USUALLY_ENDS_IN_NUCLEAR_WAR_TILE_TYPES_H
#define IT_USUALLY_ENDS_IN_NUCLEAR_WAR_TILE_TYPES_H

#include "../common/util.h"

////////////////////////////////////////////////////////////

namespace IUEINW
{
	enum Tile_Type_Grouping
	{
		ALL_TILE_TYPES = 0,
		LAND_TILE_TYPES = 1,
		OCEAN_TILE_TYPES = 2,
		NONE = 3
	};

	const int GRASS = 0;
	const int DESERT = 1;
	const int FOREST = 2;
	const int ICE = 3;
	const int PLAINS = 4;
	const int OCEAN = 5;
	const int COASTAL_OCEAN = 6;
	const int NUMBER_OF_TILE_TYPES = 7;

	const int TILE_OUTPUT_BONUS_NONE = 0;
	const int TILE_OUTPUT_BONUS_FOOD = 1;
	const int TILE_OUTPUT_BONUS_PRODUCTION = 2;

	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	// requisite stuff for getting spawn tiles
	struct SpawnTileRank {
		int Rank, TileIndex;
	};

	struct SortTilesByRank {
		bool operator()(SpawnTileRank const &a, SpawnTileRank const &b) {
			return a.Rank > b.Rank;
		}
	};

	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////

	class IUEINW_Tile_Type_Properties
	{
	public:
		void initFromJSON();

		template <class Archive>
		void serialize( Archive & ar )
		{
			ar(DefaultProductionByTileType);
			ar(DefaultFoodByTileType);
			ar(TileRanks);
			ar(SortedTileRanks_PerContinent);
		}

		std::vector<int> DefaultProductionByTileType;
		std::vector<int> DefaultFoodByTileType;
		std::vector<int> TileRanks;
		std::vector<std::vector<int>> SortedTileRanks_PerContinent; // first dimension represents ContinentIndex, second dimension represents sorted (highest to lowest) tile ranks for that continent

		int convertStringToTileType(std::string TileTypeString);
		void rankTiles();
		int rankTile(int TileIndex); // todo -- should be private
		std::vector<int> getSpawnTiles(int &NumberOfNations);
	};
}

#endif