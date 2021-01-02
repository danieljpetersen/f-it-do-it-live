#include "map_generation.h"

void IUEINW::IUEINW_Plugin_Generate_Map::smallBoard_generateTileTypes()
{
	bool savedRound = SmallBoard.Grid.HorizontalWrap;
	SmallBoard.Grid.HorizontalWrap = false;
	smallBoard_generatePlains();
	smallBoard_generateHill();
	smallBoard_generateForest();
	smallBoard_generateDesert();
	smallBoard_generateIce();
	SmallBoard.Grid.HorizontalWrap = savedRound;
}

//==================================

void IUEINW::IUEINW_Plugin_Generate_Map::smallBoard_generateForest()
{
	const int LowestRow = 0;
	const int HighestRow = SmallBoard.Grid.NumberOfRows - 1;
	//const int ModifyChance = 8; // value on oct 17, 2017
	//const int ExpandChance = 80; // value on oct 17, 2017
	const int ModifyChance = 8;
	const int ExpandChance = 85;
	const int TileType = FOREST;

	smallBoard_generateTileType(LowestRow, HighestRow, ModifyChance, ExpandChance, TileType);
}

//==================================

void IUEINW::IUEINW_Plugin_Generate_Map::smallBoard_generateDesert()
{
	int Range = fi::Math::getPercent(15, (float) SmallBoard.Grid.NumberOfRows - 1);
	const int LowestRow = ((SmallBoard.Grid.NumberOfRows) / 2) - Range;
	const int HighestRow = int(SmallBoard.Grid.NumberOfRows / 2 + Range);
	int ModifyChance = 1;
	int ExpandChance = 95;
	const int TileType = DESERT;

	smallBoard_generateTileType(LowestRow, HighestRow, ModifyChance, ExpandChance, TileType);

	ModifyChance = 1;
	ExpandChance = 30;
	smallBoard_generateTileType(LowestRow, HighestRow, ModifyChance, ExpandChance, TileType);

	ModifyChance = 0;
	ExpandChance = 66;
	smallBoard_generateTileType(LowestRow, HighestRow, ModifyChance, ExpandChance, TileType);
	if (fi::getRandom().chance(50))
	{
		smallBoard_generateTileType(LowestRow, HighestRow, ModifyChance, ExpandChance, TileType);

		if (fi::getRandom().chance(10))
			smallBoard_generateTileType(LowestRow, HighestRow, ModifyChance, ExpandChance, TileType);
	}
}

//==================================

void IUEINW::IUEINW_Plugin_Generate_Map::smallBoard_generateIce()
{
	int LowestRow = 0;
	int HighestRow = int(fi::Math::getPercent(24, (float) SmallBoard.Grid.NumberOfRows));
	getTiles().LatitudeZone_NumberOfRowsInTopAndBottom = HighestRow;

	int ModifyChance = 5;
	int ExpandChance = 25;
	const int TileType = ICE;

	int NumberOfExtraTimes = fi::getRandom().i(2, 3);

	smallBoard_generateTileType(LowestRow, HighestRow, ModifyChance, ExpandChance, TileType);
	for (int i = 0; i < NumberOfExtraTimes; i++)
		smallBoard_generateTileType(LowestRow, HighestRow, 0, ExpandChance, TileType);

	LowestRow = SmallBoard.Grid.NumberOfRows - HighestRow;
	HighestRow = SmallBoard.Grid.NumberOfRows - 1;
	smallBoard_generateTileType(LowestRow, HighestRow, ModifyChance, ExpandChance, TileType);
	for (int i = 0; i < NumberOfExtraTimes; i++)
		smallBoard_generateTileType(LowestRow, HighestRow, 0, ExpandChance, TileType);


	LowestRow = 0;
	HighestRow = 0;
	ModifyChance = 100;
	ExpandChance = 20;
	smallBoard_generateTileType(LowestRow, HighestRow, ModifyChance, ExpandChance, TileType);

	LowestRow = SmallBoard.Grid.NumberOfRows - 1;
	HighestRow = SmallBoard.Grid.NumberOfRows - 1;
	smallBoard_generateTileType(LowestRow, HighestRow, ModifyChance, ExpandChance, TileType);

	smallBoard_generateTileType(LowestRow, HighestRow, 0, ExpandChance, TileType);
	smallBoard_generateTileType(LowestRow, HighestRow, 0, ExpandChance, TileType);
}

//==================================
// small board extra tile type
void IUEINW::IUEINW_Plugin_Generate_Map::smallBoard_generateHill()
{
	const int LowestRow = 0;
	const int HighestRow = SmallBoard.Grid.NumberOfRows - 1;
	const int ModifyChance = 5;
	const int ExpandChance = 30;
	const int TileType = NUMBER_OF_TILE_TYPES;

	smallBoard_generateTileType(LowestRow, HighestRow, ModifyChance, ExpandChance, TileType);
}

//==================================

void IUEINW::IUEINW_Plugin_Generate_Map::smallBoard_generatePlains()
{
	const int LowestRow = int(fi::Math::getPercent(15, (float) SmallBoard.Grid.NumberOfRows));
	const int HighestRow = SmallBoard.Grid.NumberOfRows - 1;
	const int ModifyChance = 3;
	const int ExpandChance = 20;
	const int TileType = PLAINS;

	smallBoard_generateTileType(LowestRow, HighestRow, ModifyChance, ExpandChance, TileType);
}

//==================================

void IUEINW::IUEINW_Plugin_Generate_Map::smallBoard_generateTileType(int LowestRow, int HighestRow, int ModifyChance, int ExpandChance, int TileType)
{
	int Index = LowestRow, DiceRoll = 0, Row = LowestRow;;
	std::vector<int> TileSeeds;
	int TileSeedsIndex = 0;
	std::vector<int> TilesToExpand;

//	int i = 0;
	//Go through every tile between lowest and highest row, randomly assigning tiles to be thet ile type
	while ((SmallBoard.Grid.isValidTile(Index)) && (SmallBoard.Grid.CommonCellData[Index].Row <= HighestRow))
	{
		if (SmallBoard.Grid.CustomCellData[Index].isWaterTile() != true)
		{
			if (fi::getRandom().chance(ModifyChance))
			{
				SmallBoard.Grid.CustomCellData[Index].TileType = TileType;
				TileSeeds.resize(TileSeedsIndex + 1);
				TileSeeds[TileSeedsIndex++] = Index;
			}

			if (SmallBoard.Grid.CustomCellData[Index].TileType == TileType)
				TilesToExpand.push_back(Index);
		}

		int Neighbor = SmallBoard.Grid.getCellIndex(SmallBoard.Grid.CommonCellData[Index].Column+1, SmallBoard.Grid.CommonCellData[Index].Row);//TileToTopRight);
		if (SmallBoard.Grid.isValidTile(Neighbor))
		{
			Index = Neighbor;
		}
		else
		{
			Row++;
			Index = Row;
			if (SmallBoard.Grid.isValidTile(Index) != true)
				break;

			if (Index == SmallBoard.Grid.NumberOfRows)
				break;
		}

//		i += 1;
//		if (i >= SmallBoard.NumberOfTilesUpright)
//			break;
	}

	int TimesToLoop = TilesToExpand.size(), CenterTile = 0, Neighbor = 0;
	for (int i = 0; i < TimesToLoop; i++)
	{
		CenterTile = TilesToExpand[i];
		for (int k = 0; k < SmallBoard.Grid.Neighbors[CenterTile].size(); k++)
		{
			Neighbor = SmallBoard.Grid.Neighbors[CenterTile][k];
			if (Neighbor != -1)
			{
				if (SmallBoard.Grid.CustomCellData[Neighbor].isWaterTile() != true)
				{
					if (fi::getRandom().chance((ExpandChance)))
					{
						SmallBoard.Grid.CustomCellData[Neighbor].TileType = TileType;
					}
				}
			}
		}
	}
}