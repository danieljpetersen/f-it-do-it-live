
#include "map_generation.h"
#include "color_schemes.h"

//==================================

void IUEINW::IUEINW_Map_Generator::largeBoard_setTileTypesBasedOnSmallBoard()
{
	int Scale = MapLayout.getScale();

	getTiles().LatitudeZone_NumberOfRowsInTopAndBottom *= Scale;

	for (unsigned int i = 0; i < SmallBoard.Grid.size(); i++)
	{
		int TileType = SmallBoard.Grid.CustomCellData[i].TileType;
		int SmallRow = SmallBoard.Grid.CommonCellData[i].Row;
		int SmallCol = SmallBoard.Grid.CommonCellData[i].Column;
		int LargeRow = SmallRow * Scale;
		int LargeCol = SmallCol * Scale;

		const int OriginIndex = getTiles().Grid.getCellIndex(LargeCol, LargeRow);
		int ColIndex = LargeCol;
		int RowIndex = LargeRow;

		for (int x = 0; x < Scale; x++)
		{
			for (int y = 0; y < Scale; y++)
			{
				int Index = getTiles().Grid.getCellIndex(ColIndex + x, RowIndex + y);
				if (getTiles().Grid.isValidCell(Index))
				{
					getTiles().Grid.CustomCellData[Index].TileType = TileType;
				}
			}
		}
	}
}

//==================================

void IUEINW::IUEINW_Map_Generator::largeBoard_removeBlockinessFromTranslatedMap()
{
	std::vector<int> ChangeToWater, ChangeToLand;
	for (int i = 0; i < getTiles().Grid.size(); i++)
	{
		if (largeBoard_isCoastalTile(i))
		{
			if (fi::getRandom().chance(23))
			{
				if (getTiles().Grid.CustomCellData[i].TileType == GRASS)
					ChangeToWater.push_back(i);
				else
					ChangeToLand.push_back(i);
			}
		}
	}

	//can't do this in above loop because it'll fuck up all the shit
	for (unsigned int i = 0; i < ChangeToLand.size(); i++)
	{
		getTiles().Grid.CustomCellData[ChangeToLand[i]].TileType = GRASS;
	}
	for (unsigned int i = 0; i < ChangeToWater.size(); i++)
	{
		getTiles().Grid.CustomCellData[ChangeToWater[i]].TileType = OCEAN;
	}

	//loop through one more time and remove land tiles with more than x ocean neighbors
	ChangeToWater.resize(0);
	for (int i = 0; i < getTiles().Grid.size(); i++)
	{
		if (largeBoard_isCoastalTile(i))
		{
			int NumberOfOceanTileNeighbors = 0;
			for (unsigned int j = 0; j < getTiles().Grid.Neighbors[i].size(); j++)
			{
				int Neighbor = getTiles().Grid.Neighbors[i][j];
				if (getTiles().Grid.CustomCellData[Neighbor].isWaterTile())
				{
					NumberOfOceanTileNeighbors++;
				}
			}
			if (NumberOfOceanTileNeighbors > 4)
				ChangeToWater.push_back(i);
		}
	}

	//can't do this in above loop because it'll fuck up all the shit
	for (unsigned int i = 0; i < ChangeToWater.size(); i++)
	{
		getTiles().Grid.CustomCellData[ChangeToWater[i]].TileType = OCEAN;
	}
}

//==================================

void IUEINW::IUEINW_Map_Generator::largeBoard_setTilePropertiesBasedOnCurrentTileType()
{
	int TileType;
	for (int i = 0; i < getTiles().Grid.size(); i++)
	{
		TileType = getTiles().Grid.CustomCellData[i].TileType;


		getTiles().Grid.CustomCellData[i].aColor = getColorSchemes().getTileColor(TileType);

		// small board tile type fucks things up
		if (TileType == NUMBER_OF_TILE_TYPES)
			TileType = PLAINS;

		getTiles().Grid.CustomCellData[i].ProductionOutputBase = getMap().TileTypeProperties.DefaultProductionByTileType[TileType];
		getTiles().Grid.CustomCellData[i].FoodOutputBase = getMap().TileTypeProperties.DefaultFoodByTileType[TileType];
	}
}

//==================================

void IUEINW::IUEINW_Map_Generator::largeBoard_blendTiles()
{
	// Right now each tile has its TileType property set as one of the traditional types, IE:  GRASS, ICE, DESERT, etc.
	// in this function we're going to assign TileType (LAND, OCEAN, COASTAL), TileOutputs, and TileColor based on the blending of tiles in this function
	// Intensities are separately applied after this

	//std::vector<int> NumberOfTilesUsedInBlend;
	//std::vector<std::vector<int>> TileTypes; // so we dont modify the board in place
	//TileTypes.resize(getTiles().Grid.size());
	//NumberOfTilesUsedInBlend.resize(getTiles().Grid.size());
	//for (int i = 0; i < TileTypes.size(); i++)
	//{
	//	NumberOfTilesUsedInBlend[i] = 1;
	//
	//	TileTypes[i].resize(NUMBER_OF_TILE_TYPES+1);
	//	for (int j = 0; j < TileTypes[i].size(); j++)
	//	{
	//		TileTypes[i][j] = 0;
	//	}
	//}
	//
	//
	//const int NumberOfPasses = 5;
	//for (int Pass = 0; Pass < NumberOfPasses; Pass++)
	//{
	//	for (int TileIndex = 0; TileIndex < getTiles().Grid.size(); TileIndex++)
	//	{
	//		if (getTiles().Grid.CustomCellData[TileIndex].isWaterTile() != true)
	//		{
	//			int TileType = getTiles().Grid.CustomCellData[TileIndex].TileType;
	//			TileTypes[TileIndex][TileType] += NumberOfPasses;
	//
	//			int AreaSize;
	//			std::vector<int> *Area = getTiles().getArea(TileIndex, 7, Tile_Type_Grouping::ALL_TILE_TYPES, AreaSize);
	//			for (int i = 0; i < AreaSize; i++)
	//			{
	//				int Neighbor = Area->at(i);
	//				TileType = getTiles().Grid.CustomCellData[Neighbor].TileType;
	//				TileTypes[TileIndex][TileType] += 1;
	//				NumberOfTilesUsedInBlend[TileIndex] += 1;
	//			}
	//		}
	//	}
	//
	//	for (int TileIndex = 0; TileIndex < getTiles().Grid.size(); TileIndex++)
	//	{
	//		if (getTiles().Grid.CustomCellData[TileIndex].isWaterTile() != true)
	//		{
	//			int r = 0;
	//			int g = 0;
	//			int b = 0;
	//
	//			for (int TileTypeIndex = 0; TileTypeIndex < TileTypes[TileIndex].size(); TileTypeIndex++)
	//			{
	//				if (TileTypes[TileIndex][TileTypeIndex] > 0)
	//				{
	//					sf::Color Color = getColorSchemes().getTileColor(TileTypeIndex);
	//					r += Color.r;
	//					g += Color.g;
	//					b += Color.b;
	//
	//					//if (g < gV)
	//					//	g = gV;
	//					//if (b < bV)
	//					//	b = bV;
	//					//if (r < rV)
	//					//	r = rV;
	//				}
	//			}
	//
	//			r = r / NumberOfTilesUsedInBlend[TileIndex];
	//			g = g / NumberOfTilesUsedInBlend[TileIndex];
	//			b = b / NumberOfTilesUsedInBlend[TileIndex];
	//			getTiles().Grid.CustomCellData[TileIndex].Color = sf::Color(r, g, b);
	//		}
	//	}
	//}

	//
	//
	//	getTiles().Grid.CustomCellData[i].Color = sf::Color(r, g, b);
	//
	//	// ----
	//
	//	int PO = ProductionOutput / NumOfValidTiles;
	//	int FO = FoodOutput / NumOfValidTiles;
	//
	//	if (PO == 0)
	//	{
	//		PO = 5;
	//	}
	//
	//	if (j == 0)
	//	{
	//		getTiles().Grid.CustomCellData[i].ProductionOutput = ProductionOutput / NumOfValidTiles;
	//		getTiles().Grid.CustomCellData[i].FoodOutput = FoodOutput / NumOfValidTiles;
	//	}
	//}
	//






			//int NumberOfPasses = 5;//fi::getRandom().int_(2, 22);//4; //6 seems to look decent, play around with this if you want
	//
	//
	////loop over board and average tile colors with their neighbors x times
	//for (int j = 0; j < NumberOfPasses; j++)
	//{
	//	for (int i = 0; i < getTiles().Grid.size(); i++)
	//	{
	//		if (getTiles().Grid.CustomCellData[i].isWaterTile() != true)
	//		{
	//			int r = getTiles().Grid.CustomCellData[i].Color.r;
	//			int g = getTiles().Grid.CustomCellData[i].Color.g;
	//			int b = getTiles().Grid.CustomCellData[i].Color.b;
	//
	//			int FoodOutput = getTiles().Grid.CustomCellData[i].FoodOutput;
	//			int ProductionOutput = getTiles().Grid.CustomCellData[i].ProductionOutput;
	//
	//			std::vector<int> TileTypesUsedInBlend(NUMBER_OF_TILE_TYPES+1);
	//			for (int i = 0; i < TileTypesUsedInBlend.size(); i++)
	//				TileTypesUsedInBlend[i] = 0;
	//
	//			TileTypesUsedInBlend[getTiles().Grid.CustomCellData[i].TileType] = 1;
	//
	//			int NumOfValidTiles = 1;
	//
	//			for (unsigned int k = 0; k < getTiles().Grid.Neighbors[i].size(); k++)
	//			{
	//				int Index = getTiles().Grid.Neighbors[i][k];
	//				if ((getTiles().Grid.CustomCellData[Index].isWaterTile() != true))
	//				{
	//					NumOfValidTiles++;
	//					TileTypesUsedInBlend[getTiles().Grid.CustomCellData[Index].TileType] += 1;
	//
	//					r += getTiles().Grid.CustomCellData[Index].Color.r;
	//					g += getTiles().Grid.CustomCellData[Index].Color.g;
	//					b += getTiles().Grid.CustomCellData[Index].Color.b;
	//
	//					ProductionOutput += getTiles().Grid.CustomCellData[Index].ProductionOutput;
	//					FoodOutput += getTiles().Grid.CustomCellData[Index].FoodOutput;
	//				}
	//			}
	//
	//
	//			// ---- tile color
	//			{
	//				r = r / NumOfValidTiles;
	//				g = g / NumOfValidTiles;
	//				b = b / NumOfValidTiles;
	//
	//				if (g < gV)
	//					g = gV;
	//				if (b < bV)
	//					b = bV;
	//				if (r < rV)
	//					r = rV;
	//
	//				getTiles().Grid.CustomCellData[i].Color = sf::Color(r, g, b);
	//			}
	//
	//			// ---- tile output
	//			{
	//				int PO = ProductionOutput / NumOfValidTiles;
	//				int FO = FoodOutput / NumOfValidTiles;
	//
	//				if (PO == 0)
	//				{
	//					PO = 5;
	//				}
	//
	//				if (j == 0)
	//				{
	//					getTiles().Grid.CustomCellData[i].ProductionOutput = ProductionOutput / NumOfValidTiles;
	//					getTiles().Grid.CustomCellData[i].FoodOutput = FoodOutput / NumOfValidTiles;
	//				}
	//			}
	//		}
	//	}
	//}
}

//==================================

void IUEINW::IUEINW_Map_Generator::largeBoard_blendTileTypes()
{
	int NumberOfPasses = 5;//fi::getRandom().int_(2, 22);//4; //6 seems to look decent, play around with this if you want

	std::vector<sf::Color> Colors(getTiles().Grid.size(), sf::Color::Black);

	int rV = 12;//12
	int gV = 30;//30
	int bV = 21;//21

	//loop over board and average tile colors with their neighbors x times
	for (int j = 0; j < NumberOfPasses; j++)
	{
		for (int i = 0; i < getTiles().Grid.size(); i++)
		{
			if (getTiles().Grid.CustomCellData[i].isWaterTile() != true)
			{
				int r = getTiles().Grid.CustomCellData[i].aColor.r;
				int g = getTiles().Grid.CustomCellData[i].aColor.g;
				int b = getTiles().Grid.CustomCellData[i].aColor.b;

				int FoodOutput = getTiles().Grid.CustomCellData[i].FoodOutputBase;
				int ProductionOutput = getTiles().Grid.CustomCellData[i].ProductionOutputBase;

				std::vector<int> TileTypesUsedInBlend(NUMBER_OF_TILE_TYPES+1);
				for (int i = 0; i < TileTypesUsedInBlend.size(); i++)
					TileTypesUsedInBlend[i] = 0;

				TileTypesUsedInBlend[getTiles().Grid.CustomCellData[i].TileType] = 1;

				int NumOfValidTiles = 1;
				for (unsigned int k = 0; k < getTiles().Grid.Neighbors[i].size(); k++)
				{
					int Index = getTiles().Grid.Neighbors[i][k];
					if ((getTiles().Grid.CustomCellData[Index].isWaterTile() != true))
					{
						NumOfValidTiles++;
						TileTypesUsedInBlend[getTiles().Grid.CustomCellData[Index].TileType] += 1;

						r += getTiles().Grid.CustomCellData[Index].aColor.r;
						g += getTiles().Grid.CustomCellData[Index].aColor.g;
						b += getTiles().Grid.CustomCellData[Index].aColor.b;

						ProductionOutput += getTiles().Grid.CustomCellData[Index].ProductionOutputBase;
						FoodOutput += getTiles().Grid.CustomCellData[Index].FoodOutputBase;
					}
				}


				// ---- tile color
				{
					r = r / NumOfValidTiles;
					g = g / NumOfValidTiles;
					b = b / NumOfValidTiles;

					if (g < gV)
						g = gV;
					if (b < bV)
						b = bV;
					if (r < rV)
						r = rV;

					Colors[i] = sf::Color(r, g, b);
				}

				// ---- tile output
				{
					int PO = ProductionOutput / NumOfValidTiles;
					int FO = FoodOutput / NumOfValidTiles;

					if (PO == 0)
					{
						PO = 5;
					}

					if (j == 0)
					{
						getTiles().Grid.CustomCellData[i].ProductionOutputBase = ProductionOutput / NumOfValidTiles;
						getTiles().Grid.CustomCellData[i].FoodOutputBase = FoodOutput / NumOfValidTiles;
					}
				}
			}
		}

		for (int i = 0; i < Colors.size(); i++)
		{
			if (getTiles().Grid.CustomCellData[i].isWaterTile() != true)
			{
				getTiles().Grid.CustomCellData[i].aColor = Colors[i];
				Colors[i] = sf::Color::Black;
			}
		}
	}
}

//==================================

void IUEINW::IUEINW_Map_Generator::largeBoard_setTileIntensitiesAkaBonusOutput()
{
	std::vector<int> Intensities;
	std::list<int> UnlockedTiles;

	// ---- all non-water tiles to unlocked list
	{
		for (int i = 0; i < getTiles().Grid.size(); i++)
		{
			Intensities.push_back(0);

			if (getTiles().Grid.CustomCellData[i].isWaterTile() != true)
			{
				UnlockedTiles.push_back(i);
			}
		}
	}

	// ---- increment intensities until all are locked
	while (UnlockedTiles.size())
	{
		std::list<int>::iterator iter = UnlockedTiles.begin();
		while (iter != UnlockedTiles.end())
		{
			if (fi::getRandom().chance(50))
			{
				UnlockedTiles.erase(iter++);
			}
			else
			{
				int TileIndex = (*iter);
				Intensities[TileIndex] += 2;

				for (int j = 0; j < getTiles().Grid.Neighbors[TileIndex].size(); j++)
				{
					int Neighbor = getTiles().Grid.Neighbors[TileIndex][j];
					if (getTiles().Grid.CustomCellData[Neighbor].isWaterTile() != true)
					{
						Intensities[Neighbor] += 1;
					}
				}
				++iter;
			}
		}
	}

	{
		int Max = 0, Min = 999;
		for (int i = 0; i < getTiles().Grid.size(); i++)
		{
			if (Intensities[i] != 0)
			{
				if (Intensities[i] > Max)
					Max = Intensities[i];
				if (Intensities[i] < Min)
					Min = Intensities[i];
			}
		}
	}

	// ---- smooth out
	{

	}

	// ---- set colors from intensity
	{
		for (int i = 0; i < getTiles().Grid.size(); i++)
		{
			if (getTiles().Grid.CustomCellData[i].isWaterTile() != true)
			{
				int Intensity = Intensities[i];
				if (Intensity < 9)
				{
					getTiles().Grid.CustomCellData[i].aColor = getColorSchemes().calculateDarkerColor_NonShadow(getTiles().Grid.CustomCellData[i].aColor);
					getTiles().Grid.CustomCellData[i].OutputBonusBase = TILE_OUTPUT_BONUS_PRODUCTION;
					getTiles().Grid.CustomCellData[i].ProductionOutputBase += 2;
					//getTiles().Grid.CustomCellData[i].FoodOutput -= 2;
				}
				else if (Intensity < 19)
				{
					// nothing
					getTiles().Grid.CustomCellData[i].OutputBonusBase = TILE_OUTPUT_BONUS_NONE;
				}
				else
				{
					getTiles().Grid.CustomCellData[i].aColor = getColorSchemes().calculateLighterColor_NonShadow(getTiles().Grid.CustomCellData[i].aColor);
					getTiles().Grid.CustomCellData[i].OutputBonusBase = TILE_OUTPUT_BONUS_FOOD;
					//getTiles().Grid.CustomCellData[i].ProductionOutput -= 2;
					getTiles().Grid.CustomCellData[i].FoodOutputBase += 2;
				}
			}
		}
	}
}

//==================================

void IUEINW::IUEINW_Map_Generator::largeBoard_finalTileTypeAssignment()
{
	std::vector<sf::Color> MasterTileColors;
	const int INDEXES[] = { GRASS, FOREST, ICE, DESERT, PLAINS };
	MasterTileColors.push_back(getColorSchemes().getTileColor(INDEXES[0]));
	MasterTileColors.push_back(getColorSchemes().getTileColor(INDEXES[1]));
	MasterTileColors.push_back(getColorSchemes().getTileColor(INDEXES[2]));
	MasterTileColors.push_back(getColorSchemes().getTileColor(INDEXES[3]));
	MasterTileColors.push_back(getColorSchemes().getTileColor(INDEXES[4]));

	std::deque<int> TilesInNeedOfAdditionalProcessing;
	for (int i = 0; i < getTiles().Grid.size(); i++)
	{
		if (getTiles().Grid.CustomCellData[i].isWaterTile() != true)
		{
			sf::Color TileColor = getTiles().Grid.CustomCellData[i].aColor;

			int ClosestIndex = -1;
			double Closest = -1;
			for (unsigned int j = 0; j < MasterTileColors.size(); j++)
			{
				double Temp = fi::colorRankCloseness(MasterTileColors[j], TileColor);
				if ((Temp < Closest) || (ClosestIndex == -1))
				{
					Closest = Temp;
					ClosestIndex = j;
				}
			}

			getTiles().Grid.CustomCellData[i].TileType = INDEXES[ClosestIndex];

			if (INDEXES[ClosestIndex] == ICE)
			{
				TilesInNeedOfAdditionalProcessing.push_back(i);
			}
		}
	}

	// ---- this block is to fix a potential issue where tiles which should have been set to ice were interpreted to be desert
	{
		while (TilesInNeedOfAdditionalProcessing.size())
		{
			int i = TilesInNeedOfAdditionalProcessing.front();
			TilesInNeedOfAdditionalProcessing.pop_front();

			for (int j = 0; j < getTiles().Grid.Neighbors[i].size(); j++)
			{
				int Neighbor = getTiles().Grid.Neighbors[i][j];
				if (getTiles().Grid.CustomCellData[Neighbor].TileType == DESERT)
				{
					getTiles().Grid.CustomCellData[Neighbor].TileType = PLAINS;
					TilesInNeedOfAdditionalProcessing.push_back(Neighbor);
				}
			}
		}
	}


	// ---- delete me? this explicitly sets tile type colors to their actual tile type
	//{
	//	for (int i = 0; i < getTiles().Grid.size(); i++)
	//	{
	//		sf::Color Color = getColorSchemes().get(getTiles().Grid.CustomCellData[i].TileType);
	//		getTiles().Grid.CustomCellData[i].Color = Color;
	//	}
	//}
}

//==================================

void IUEINW::IUEINW_Map_Generator::largeBoard_setCoastalTiles()
{
	sf::Color Color = getColorSchemes().getTileColor(COASTAL_OCEAN);

	for (int i = 0; i < getTiles().Grid.size(); i++)
	{
		// we NEED this check, as the isCoastalTile function will return true for land coastal tiles as well
		if (getTiles().Grid.CustomCellData[i].TileType == OCEAN)
		{
			if (largeBoard_isCoastalTile(i))
			{
				getTiles().Grid.CustomCellData[i].TileType = COASTAL_OCEAN;
				getTiles().Grid.CustomCellData[i].aColor = Color;
			}
		}
	}

	std::vector<int> TilesToChange;
	// ---- expand
	{
		for (int i = 0; i < getTiles().Grid.size(); i++)
		{
			// we NEED this check, as the isCoastalTile function will return true for land coastal tiles as well
			if (getTiles().Grid.CustomCellData[i].TileType == OCEAN)
			{
				bool Coastal = false;
				for (int j = 0; j < getTiles().Grid.Neighbors[i].size(); j++)
				{
					if (getTiles()[getTiles().Grid.Neighbors[i][j]].TileType != OCEAN)
					{
						Coastal = true;
						break;
					}
				}

				if (Coastal)
				{
					TilesToChange.push_back(i);
				}
			}
		}

		Color = fi::colorAverage(Color, getColorSchemes().getTileColor(OCEAN));
		//std::vector<sf::Color> c {Color, Color, getColorSchemes().getTileColor(OCEAN)};
		//Color = fi::Color::Functions::blend(c);
		//Color = fi::Color::Functions::modify(Color, -10, -10, -10, 0);
		//Color = fi::Color::Functions::modify(Color, 10, 10, 10, 0);
		for (int i = 0; i < TilesToChange.size(); i++)
		{
			getTiles().Grid.CustomCellData[TilesToChange[i]].TileType = COASTAL_OCEAN;
			getTiles().Grid.CustomCellData[TilesToChange[i]].aColor = Color;
		}
	}

	//
	//// ---- expand
	//{
	//	std::vector<int> TilesToChange2;
	//	for (int i = 0; i < getTiles().Grid.size(); i++)
	//	{
	//		// we NEED this check, as the isCoastalTile function will return true for land coastal tiles as well
	//		if (getTiles().Grid.CustomCellData[i].TileType == OCEAN)
	//		{
	//			bool Coastal = false;
	//			for (int j = 0; j < getTiles().Grid.Neighbors[i].size(); j++)
	//			{
	//				if (getTiles()[getTiles().Grid.Neighbors[i][j]].TileType != OCEAN)
	//				{
	//					Coastal = true;
	//					break;
	//				}
	//			}
	//
	//			if (Coastal)
	//			{
	//				TilesToChange2.push_back(i);
	//			}
	//		}
	//	}
	//
	//	Color = fi::colorAverage(Color, getColorSchemes().getTileColor(OCEAN));
	//	//Color = fi::Color::Functions::modify(Color, -10, -10, -10, 0);
	//	//Color = fi::Color::Functions::modify(Color, 10, 10, 10, 0);
	//	//Color = fi::getRandom().color();
	//	for (int i = 0; i < TilesToChange2.size(); i++)
	//	{
	//		//getTiles().Grid.CustomCellData[TilesToChange2[i]].TileType = COASTAL_OCEAN;
	//		getTiles().Grid.CustomCellData[TilesToChange2[i]].Color = Color;
	//	}
	//}
}

//==================================

bool IUEINW::IUEINW_Map_Generator::largeBoard_isCoastalTile(int Index)
{
	// NOTE:  this function is legit, and returns true if land tile is coastal, or water tile is coastal
	bool CheckingAgainstLand = true;

	// if the tile in question is a land tile, we check against ocean
	if (getTiles().Grid.CustomCellData[Index].isWaterTile() != true)
		CheckingAgainstLand = false;

	int Neighbor;
	for (int i = 0; i < getTiles().Grid.Neighbors[Index].size(); i++)
	{
		Neighbor = getTiles().Grid.Neighbors[Index][i];
		if (CheckingAgainstLand)
		{
			if (getTiles().Grid.CustomCellData[Neighbor].isWaterTile() != true)
			{
				return true;
			}
		}
		else
		{
			if (getTiles().Grid.CustomCellData[Neighbor].isWaterTile())
			{
				return true;
			}
		}
	}

	return false;
}

//==================================

void IUEINW::IUEINW_Map_Generator::largeBoard_labelLatitudeZones()
{
	for (int i = 0; i < getTiles().Grid.size(); i++)
	{
		int Zone;
		int Row = getTiles().Grid.CommonCellData[i].Row;
		if (Row <= getTiles().LatitudeZone_NumberOfRowsInTopAndBottom)
		{
			Zone = 0;
		}
		else if (Row >= getTiles().Grid.NumberOfRows - getTiles().LatitudeZone_NumberOfRowsInTopAndBottom)
		{
			Zone = 2;
		}
		else
		{
			Zone = 1;
		}

		getTiles().Grid.CustomCellData[i].LatitudeZone = Zone;
	}
}

//==================================