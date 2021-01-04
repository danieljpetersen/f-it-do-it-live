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

////////////////////////////////////////////////////////////

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