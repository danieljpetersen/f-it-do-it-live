#include "globals.h"
#include "map.h"
#include "map_generation.h"
#include "color_schemes.h"
#include "vision.h"
#include "iueinw.h"

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Tiles::init(int ColCount, int RowCount)
{
	nlohmann::json MapSection = fi::getConfig()["game"]["map"].get<nlohmann::json>();
	const int TILE_HEIGHT = MapSection["tile-width"].get<int>();
	const int TILE_WIDTH = MapSection["tile-height"].get<int>();
	bool RoundWorld = MapSection["round-world"].get<bool>();
	const int ColumnWrapThreshold = MapSection["column-wrap-threshold"].get<int>();
	Y_TILE_SLOPE_OFFSET = MapSection["tile-height-offset"].get<float>();
	
	int GridType = fi::GRID_TYPE_ISOMETRIC;
	if ((fi::stringDowncase(MapSection["grid-type"].get<std::string>()) == "square") || (fi::stringDowncase(MapSection["grid-type"].get<std::string>()) == "rectangle"))
	{
		GridType = fi::GRID_TYPE_SQUARE;
	}
	
	int GridSpacing = MapSection["grid-spacing"].get<int>();
	Grid.init(ColCount, RowCount, GridType, GridSpacing, TILE_WIDTH, TILE_HEIGHT, RoundWorld);
	
	for (int i = 0; i < Grid.CustomCellData.size(); i++)
	{
		Grid.CustomCellData[i].TileIndex = i;
		Grid.CustomCellData[i].OutputBonusBase = 0;
		Grid.CustomCellData[i].ProductionOutputBase = 0;
		Grid.CustomCellData[i].FoodOutputBase = 0;
		Grid.CustomCellData[i].NationIndex = -1;
		Grid.CustomCellData[i].ContinentIndex = -1;
		Grid.CustomCellData[i].BodyOfWaterIndex = -1;
	}
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Tiles::isPointInsideTile(int x, int y, int Index)
{
	//you can find out if a point is inside a polygon by drawing a straight line from the point to another arbitrary point and counting how many times this intersects with the the polygons sides.  if an odd number of times, it's inside, even and it's outside
	sf::Vector2f Left	= Grid.CommonCellData[Index].getLeft2f();
	sf::Vector2f Right	= Grid.CommonCellData[Index].getRight2f();
	sf::Vector2f Up		= Grid.CommonCellData[Index].getBack2f();
	sf::Vector2f Down	= Grid.CommonCellData[Index].getFront2f();
	
	sf::Vector2f Point1((float)x, (float)y);
	sf::Vector2f Point2(float(x + fi::getCanvasWorld().getSize().x), (float)y + Grid.TileHeight);
	
	int Intersections = 0;
	if (fi::Math::doLineSegmentsIntersect(Point1, Point2, Left, Up) == true)
		Intersections++;
	if (fi::Math::doLineSegmentsIntersect(Point1, Point2, Left, Down) == true)
		Intersections++;
	if (fi::Math::doLineSegmentsIntersect(Point1, Point2, Right, Up) == true)
		Intersections++;
	if (fi::Math::doLineSegmentsIntersect(Point1, Point2, Right, Down) == true)
		Intersections++;
	
	if ((Intersections == 1) )
		return true;
	else
	{
		//seriously optimize th is shit
		Point1 = sf::Vector2f((float)x + 1, (float)y + 1);
		Intersections = 0;
		if (fi::Math::doLineSegmentsIntersect(Point1, Point2, Left, Up) == true)
			Intersections++;
		if (fi::Math::doLineSegmentsIntersect(Point1, Point2, Left, Down) == true)
			Intersections++;
		if (fi::Math::doLineSegmentsIntersect(Point1, Point2, Right, Up) == true)
			Intersections++;
		if (fi::Math::doLineSegmentsIntersect(Point1, Point2, Right, Down) == true)
			Intersections++;
		if ((Intersections == 1))
			return true;
		
		return false;
	}
}

bool IUEINW::IUEINW_Tiles::areSameContinent(int aTileIndex, int bTileIndex)
{
	return Grid.CustomCellData[aTileIndex].ContinentIndex == Grid.CustomCellData[bTileIndex].ContinentIndex;
}

////////////////////////////////////////////////////////////

int IUEINW::IUEINW_Tiles::randomLandTile()
{
	return LandTiles[fi::getRandom().i(0, LandTiles.size()-1)];
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Tiles::collectTilesInColumn_WithoutWaterTiles(int ColIndex, int StartingRow, int EndingRow, std::vector<int> &TileCollection)
{
	fi::Grid_1D_Custom_Data<IUEINW_Tile> *Grid = &getTiles().Grid;
	
	int TileIndex = Grid->getCellIndex(ColIndex, StartingRow);
	int EndingTile = Grid->getCellIndex(ColIndex, EndingRow);
	while (TileIndex != EndingTile)
	{
		if (Grid->CustomCellData[TileIndex].isWaterTile() != true)
		{
			TileCollection.push_back(TileIndex);
		}
		
		TileIndex++;
	}
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Tiles::collectTilesInColumn(int ColIndex, int StartingRow, int EndingRow, std::vector<int> &TileCollection)
{
	fi::Grid_1D_Custom_Data<IUEINW_Tile> *Grid = &getTiles().Grid;

	int TileIndex = Grid->getCellIndex(ColIndex, StartingRow);
	int EndingTile = Grid->getCellIndex(ColIndex, EndingRow);
	while (TileIndex != EndingTile)
	{
		TileCollection.push_back(TileIndex);
		TileIndex++;
	}
}

////////////////////////////////////////////////////////////

std::vector<int> *IUEINW::IUEINW_Tiles::getArea(int Center, int HalfSize, Tile_Type_Grouping TileTypeGrouping, int &Size)
{
	if (TileTypeGrouping == Tile_Type_Grouping::ALL_TILE_TYPES)
	{
		Size = AreaCacheIndexMap_All[Center][HalfSize-1];
		return &AreaCache_All[Center];
	}
	if (TileTypeGrouping == Tile_Type_Grouping::LAND_TILE_TYPES)
	{
		Size = AreaCacheIndexMap_Land[Center][HalfSize-1];
		return &AreaCache_Land[Center];
	}
	if (TileTypeGrouping == Tile_Type_Grouping::OCEAN_TILE_TYPES)
	{
		Size = AreaCacheIndexMap_Water[Center][HalfSize-1];
		return &AreaCache_Water[Center];
	}
	
	Size = 0;
	return nullptr;
}

////////////////////////////////////////////////////////////

std::vector<int> IUEINW::IUEINW_Tiles::getAreaSpreadingFromCenter(int Start, int DistanceCutoff, bool WithOceanTiles)
{
	std::vector<int> CoT;
	std::deque<int> ToCheck;
	std::deque<int> Checked;
	ToCheck.push_back(Start);
	while (ToCheck.size() > 0)
	{
		int Center = ToCheck.front();
		ToCheck.pop_front();
		Checked.push_back(Center);
		CoT.push_back(Center);
		
		// todo rewrite this whole function
		
		for (unsigned int i = 0; i < Grid.Neighbors[Center].size(); i++)
		{
			int Neighbor = Grid.Neighbors[Center][i];
			if (Grid.getDistanceBetweenTiles(Start, Neighbor) < DistanceCutoff)
			{
				bool Eligible = false;
				
				if (WithOceanTiles)
				{
					Eligible = true;
				}
				else if (Grid.CustomCellData[Neighbor].isWaterTile() != true)
				{
					Eligible = true;
				}
				
				if (Eligible)
				{
					bool OnList = false;
					for (unsigned int j = 0; j < ToCheck.size(); j++)
					{
						if (ToCheck[j] == Neighbor)
						{
							OnList = true;
							break;
						}
					}
					if (OnList != true)
					{
						for (unsigned int j = 0; j < Checked.size(); j++)
						{
							if (Checked[j] == Neighbor)
							{
								OnList = true;
								break;
							}
						}
					}
					if (OnList != true)
					{
						ToCheck.push_back(Neighbor);
					}
				}
			}
		}
	}
	
	return CoT;
}

////////////////////////////////////////////////////////////
//RETURNS INDEX FOR CoT [CoT.getTile(result)] --  In other words, use return CoT.getTile(result) to get the actual Tiles.Tiles index-- GOD BLESS ME
int IUEINW::IUEINW_Tiles::findClosestTileFromTileIndex_returnsLocalIndex(std::vector<int> &TileList, int GoalTile)
{
	return findClosestTileFromTileIndex_returnsLocalIndex(TileList, GoalTile, true);
}

////////////////////////////////////////////////////////////

int IUEINW::IUEINW_Tiles::findClosestTileFromTileIndex_returnsLocalIndex(std::vector<int> &TileList, int GoalTile, bool IncludeWaterTilesInSearch)
{
	int TestTile, BestIndex = -1, BestDistance = std::numeric_limits<int>::max(), Distance = -1;
	for (unsigned int i = 0; i < TileList.size(); i++)
	{
		TestTile = TileList[i];
		if ((IncludeWaterTilesInSearch) || (Grid.CustomCellData[TestTile].isWaterTile() != true))
		{
			Distance = Grid.getDistanceBetweenTiles(GoalTile, TestTile);
			if (Distance < BestDistance)
			{
				BestDistance = Distance;
				BestIndex = i;
			}
		}
	}
	
	if (BestIndex == -1)
	{
		return -1;
	}
	
	return BestIndex;
}

////////////////////////////////////////////////////////////

std::string IUEINW::IUEINW_Tiles::convertTileTypeToString(int TileType)
{
	if (TileType == GRASS)
	{
		return "grass";
	}
	else if (TileType == ICE)
	{
		return "ice";
	}
	else if (TileType == FOREST)
	{
		return "forest";
	}
	else if (TileType == DESERT)
	{
		return "desert";
	}
	else if (TileType == OCEAN)
	{
		return "ocean";
	}
	else if (TileType == COASTAL_OCEAN)
	{
		return "coastal";
	}
	else if (TileType == PLAINS)
	{
		return "plains";
	}
	
	return "";
}

////////////////////////////////////////////////////////////

int IUEINW::IUEINW_Tiles::convertTileTypeStringToInt(std::string TileType)
{
	if ((TileType == "grass") || (TileType == "grassland"))
	{
		return GRASS;
	}
	else if (TileType == "ice")
	{
		return ICE;
	}
	else if (TileType == "forest")
	{
		return FOREST;
	}
	else if (TileType == "desert")
	{
		return DESERT;
	}
	else if (TileType == "ocean")
	{
		return OCEAN;
	}
	else if (TileType == "coastal")
	{
		return COASTAL_OCEAN;
	}
	else if (TileType == "plains")
	{
		return PLAINS;
	}
	
	return -1;
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Tiles::populateAreaCache()
{
	AreaCache_All.clear();
	AreaCache_All.resize((unsigned long)Grid.size());
	AreaCacheIndexMap_All.clear();
	AreaCacheIndexMap_All.resize((unsigned long)Grid.size());
	
	AreaCache_Land.clear();
	AreaCache_Land.resize((unsigned long)Grid.size());
	AreaCacheIndexMap_Land.clear();
	AreaCacheIndexMap_Land.resize((unsigned long)Grid.size());
	
	
	AreaCache_Water.clear();
	AreaCache_Water.resize((unsigned long)Grid.size());
	AreaCacheIndexMap_Water.clear();
	AreaCacheIndexMap_Water.resize((unsigned long)Grid.size());
	
	MAX_CACHE_AREA = 18;
	// todo
	// if (CITY_LINK_PATH_MAX_SIZE > MAX_CACHE_AREA)
	// {
	// 	MAX_CACHE_AREA = CITY_LINK_PATH_MAX_SIZE;
	// }
	//
	// // ---- find max_area that we'll need as defined in the config
	// {
	// 	json GameplaySection = fi::jsonConfig["gameplay"].get<json>();
	// 	json CityContent = GameplaySection["city"].get<json>();
	// 	if (CityContent["city-vision-size-upon-founding"].get<int>() > MAX_CACHE_AREA)
	// 	{
	// 		MAX_CACHE_AREA = CityContent["city-vision-size-upon-founding"].get<int>();
	// 	}
	//
	// 	if (CityContent["starting-city-size"].get<int>() > MAX_CACHE_AREA)
	// 	{
	// 		MAX_CACHE_AREA = CityContent["starting-city-size"].get<int>();
	// 	}
	//
	// 	if (INITIAL_SPAWN_AREA_SEARCH_SIZE > MAX_CACHE_AREA)
	// 	{
	// 		MAX_CACHE_AREA = INITIAL_SPAWN_AREA_SEARCH_SIZE;
	// 	}
	//
	// 	auto UnitsSection = GameplaySection["units"].get<json>();
	// 	auto UnitsArray = UnitsSection["types"].get<json>();
	//
	// 	for (json::iterator it = UnitsArray.begin(); it != UnitsArray.end(); ++it)
	// 	{
	// 		int VisionRange = (*it)["vision-range"].get<float>();
	// 		int AttackRange = (*it)["attack-range"].get<float>();
	//
	// 		if (VisionRange > MAX_CACHE_AREA)
	// 		{
	// 			MAX_CACHE_AREA = VisionRange;
	// 		}
	// 		if (AttackRange > MAX_CACHE_AREA)
	// 		{
	// 			MAX_CACHE_AREA = AttackRange;
	// 		}
	// 	}
	// }
	
	for (int i = 0; i < Grid.size(); i++)
	{
		AreaCache_All[i] = calculateArea_ImplementationTwo(i, MAX_CACHE_AREA, Tile_Type_Grouping::ALL_TILE_TYPES);
		AreaCache_Land[i] = calculateArea_ImplementationTwo(i, MAX_CACHE_AREA, Tile_Type_Grouping::LAND_TILE_TYPES);
		AreaCache_Water[i] = calculateArea_ImplementationTwo(i, MAX_CACHE_AREA, Tile_Type_Grouping::OCEAN_TILE_TYPES);
	}
	
	// ---- populate edge tiles
	{
		EdgeTiles.clear();
		EdgeTiles.resize(4);
		
		// ---- bottom left edge
		{
			int Row = 0;
			int Col = 0;
			
			while (Row < Grid.NumberOfRows)
			{
				EdgeTiles[BOTTOMLEFT_EDGE].push_back(Grid.getCellIndex(Col, Row));
				Row++;
			}
		}
		
		// ---- bottom right edge
		{
			int Row = Grid.NumberOfRows-1;
			int Col = 0;
			
			while (Col < Grid.NumberOfColumns)
			{
				EdgeTiles[BOTTOMRIGHT_EDGE].push_back(Grid.getCellIndex(Col, Row));
				Col++;
			}
		}
		
		// ---- top left edge
		{
			int Row = 0;
			int Col = 0;
			
			while (Col < Grid.NumberOfColumns)
			{
				EdgeTiles[TOPLEFT_EDGE].push_back(Grid.getCellIndex(Col, Row));
				Col++;
			}
		}
		
		// ---- top right edge
		{
			int Row = 0;
			int Col = Grid.NumberOfColumns-1;
			
			while (Row < Grid.NumberOfRows)
			{
				EdgeTiles[TOPRIGHT_EDGE].push_back(Grid.getCellIndex(Col, Row));
				Row++;
			}
		}
	}
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Tiles::determineContinents()
{
	Continents.clear();
	
	std::vector<bool> Processed(Grid.NumberOfCells);
	for (int j = 0; j < Grid.NumberOfCells; j++)
	{
		if (Grid.CustomCellData[j].isWaterTile())
			Processed[j] = true;
		else
			Processed[j] = false;
	}
	
	int NumberOfContinents = 0;
	for (int i = 0; i < Grid.NumberOfCells; i++)
	{
		if (!Processed[i])
		{
			std::list<int> TilesToAssign(0);
			TilesToAssign.push_back(i);
			
			while (!TilesToAssign.empty())
			{
				int TileIndex = TilesToAssign.front();
				for (int j = 0; j < Grid.Neighbors[TileIndex].size(); j++)
				{
					int Neighbor = Grid.Neighbors[TileIndex][j];
					if (Neighbor != -1)
					{
						if (!Processed[Neighbor])
						{
							TilesToAssign.push_back(Neighbor);
							Processed[Neighbor] = true;
						}
					}
				}
				
				Grid.CustomCellData[TilesToAssign.front()].ContinentIndex = NumberOfContinents;
				Processed[TilesToAssign.front()] = true;
				TilesToAssign.pop_front();
			}
			
			NumberOfContinents++;
		}
	}
	
	Continents.clear();
	Continents.resize(NumberOfContinents);
	for (int i = 0; i < Grid.NumberOfCells; i++)
	{
		if (Grid.CustomCellData[i].isWaterTile() != true)
			Continents[Grid.CustomCellData[i].ContinentIndex].push_back(i);
	}
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Tiles::determineBodiesOfWater()
{
	BodiesOfWater.clear();
	
	std::vector<bool> Processed(Grid.NumberOfCells);
	for (int j = 0; j < Grid.NumberOfCells; j++)
	{
		if (Grid.CustomCellData[j].isWaterTile() != true)
			Processed[j] = true;
		else
			Processed[j] = false;
	}
	
	int NumberOfBodiesOfWater = 0;
	for (int i = 0; i < Grid.NumberOfCells; i++)
	{
		if (!Processed[i])
		{
			std::list<int> TilesToAssign(0);
			TilesToAssign.push_back(i);
			
			while (!TilesToAssign.empty())
			{
				int TileIndex = TilesToAssign.front();
				for (int j = 0; j < Grid.Neighbors[TileIndex].size(); j++)
				{
					int Neighbor = Grid.Neighbors[TileIndex][j];
					if (Neighbor != -1)
					{
						if (!Processed[Neighbor])
						{
							TilesToAssign.push_back(Neighbor);
							Processed[Neighbor] = true;
						}
					}
				}
				
				Grid.CustomCellData[TilesToAssign.front()].BodyOfWaterIndex = (NumberOfBodiesOfWater);
				Processed[TilesToAssign.front()] = true;
				TilesToAssign.pop_front();
			}
			
			NumberOfBodiesOfWater++;
		}
	}
	
	BodiesOfWater.clear();
	BodiesOfWater.resize(NumberOfBodiesOfWater);
	for (int i = 0; i < Grid.NumberOfCells; i++)
	{
		if (Grid.CustomCellData[i].isWaterTile())
			BodiesOfWater[Grid.CustomCellData[i].BodyOfWaterIndex].push_back(i);
	}
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Tiles::determineLandAndWaterTiles()
{
	LandTiles.clear();
	WaterTiles.clear();
	AllTiles.clear();
	
	for (int i = 0; i < Grid.NumberOfCells; i++)
	{
		if (Grid.CustomCellData[i].isWaterTile())
		{
			WaterTiles.push_back(i);
		}
		else
		{
			LandTiles.push_back(i);
		}
		AllTiles.push_back(i);
	}
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Tiles::offsetTileCoordinatesFromHeight()
{
	for (int Index = 0; Index < this->size(); Index++)
	{
		sf::Vector2f NewLeft = Grid.CommonCellData[Index].getLeft2f();
		sf::Vector2f NewRight = Grid.CommonCellData[Index].getRight2f();
		sf::Vector2f NewBack = Grid.CommonCellData[Index].getBack2f();
		sf::Vector2f NewFront = Grid.CommonCellData[Index].getFront2f();
		
		NewLeft.y -= getTiles().Y_TILE_SLOPE_OFFSET * Grid.CustomCellData[Index].Height[LEFT];
		NewRight.y -= getTiles().Y_TILE_SLOPE_OFFSET * Grid.CustomCellData[Index].Height[RIGHT];
		NewFront.y -= getTiles().Y_TILE_SLOPE_OFFSET * Grid.CustomCellData[Index].Height[FRONT];
		NewBack.y -= getTiles().Y_TILE_SLOPE_OFFSET * Grid.CustomCellData[Index].Height[BACK];
		
		//set height of this tile
		Grid.CommonCellData[Index].Corners[LEFT] = NewLeft;
		Grid.CommonCellData[Index].Corners[RIGHT] = NewRight;
		Grid.CommonCellData[Index].Corners[FRONT] = NewFront;
		Grid.CommonCellData[Index].Corners[BACK] = NewBack;
	}
}

////////////////////////////////////////////////////////////
// order is different -- goes column by column
std::vector<int> IUEINW::IUEINW_Tiles::calculateArea_ImplementationOne(int Center, int NumberOfRings, bool WithWaterTiles)
{
	std::vector<int> Result;
	
	int CenterRow = Grid.CommonCellData[Center].Row;
	int CenterCol = Grid.CommonCellData[Center].Column;
	
	int StartingRow = std::max(CenterRow-NumberOfRings, 0);
	int EndingRow   = std::min(CenterRow+NumberOfRings+1, Grid.NumberOfRows);
	
	void (*collectTiles)(int, int, int, std::vector<int> &) = nullptr;
	if (WithWaterTiles)
	{
		collectTiles = &collectTilesInColumn;
	}
	else
	{
		collectTiles = &collectTilesInColumn_WithoutWaterTiles;
	}
	
	// ---- collect center column tiles
	collectTiles(CenterCol, StartingRow, EndingRow, Result);
	
	// ---- collect tiles to left
	{
		int Col;
		int TileIndex = Center;
		for (int i = 0; i < NumberOfRings; i++)
		{
			TileIndex = Grid.getTileNeighbor(TileIndex, TileToLeft);
			if (Grid.isValidTile(TileIndex) != true)
			{
				break;
			}
			
			Col = Grid.CommonCellData[TileIndex].Column;
			collectTiles(Col, StartingRow, EndingRow, Result);
		}
	}
	
	// ---- collect tiles to right
	{
		int Col;
		int TileIndex = Center;
		for (int i = 0; i < NumberOfRings; i++)
		{
			TileIndex = Grid.getTileNeighbor(TileIndex, TileToRight);
			if (Grid.isValidTile(TileIndex) != true)
			{
				break;
			}
			
			Col = Grid.CommonCellData[TileIndex].Column;
			collectTiles(Col, StartingRow, EndingRow, Result);
		}
	}
	
	return Result;
}

////////////////////////////////////////////////////////////
// order is different -- goes in rings
std::vector<int> IUEINW::IUEINW_Tiles::calculateArea_ImplementationTwo(int Center, int NumberOfRings, Tile_Type_Grouping TileTypeGrouping)
{
	std::vector<int> Result;
	Result.push_back(Center);
	
	int RingSize, TileIndex;
	int Row = Grid.CommonCellData[Center].Row;
	int Col = Grid.CommonCellData[Center].Column;
	
	for (int i = 0; i < NumberOfRings; i++)
	{
		RingSize = (i+1)*2;
		Row -= 1;
		Col -= 1;
		
		// ---- top
		{
			for (int j = 0; j < RingSize; j++)
			{
				Col++;
				if (Col >= Grid.NumberOfColumns)
				{
					if (Grid.HorizontalWrap)
					{
						Col = 0;
					}
				}
				
				TileIndex = Grid.getCellIndex_ValidateCoordinates(Col, Row);
				if (Grid.isValidCell(TileIndex))
				{
					if ((TileTypeGrouping == Tile_Type_Grouping::ALL_TILE_TYPES) ||
						((TileTypeGrouping == Tile_Type_Grouping::LAND_TILE_TYPES) && (Grid.CustomCellData[TileIndex].isWaterTile() != true)) ||
						((TileTypeGrouping == Tile_Type_Grouping::OCEAN_TILE_TYPES) && (Grid.CustomCellData[TileIndex].isWaterTile())))
					{
						Result.push_back(TileIndex);
					}
				}
			}
		}
		
		// ---- right
		{
			for (int j = 0; j < RingSize; j++)
			{
				Row++;
				
				TileIndex = Grid.getCellIndex_ValidateCoordinates(Col, Row);
				if (Grid.isValidCell(TileIndex))
				{
					if ((TileTypeGrouping == Tile_Type_Grouping::ALL_TILE_TYPES) ||
						((TileTypeGrouping == Tile_Type_Grouping::LAND_TILE_TYPES) && (Grid.CustomCellData[TileIndex].isWaterTile() != true)) ||
						((TileTypeGrouping == Tile_Type_Grouping::OCEAN_TILE_TYPES) && (Grid.CustomCellData[TileIndex].isWaterTile())))
					{
						Result.push_back(TileIndex);
					}
				}
			}
		}
		
		// ---- bottom
		{
			for (int j = 0; j < RingSize; j++)
			{
				Col--;
				if (Col < 0)
				{
					if (Grid.HorizontalWrap)
					{
						Col = Grid.NumberOfColumns-1;
					}
				}
				
				TileIndex = Grid.getCellIndex_ValidateCoordinates(Col, Row);
				if (Grid.isValidCell(TileIndex))
				{
					if ((TileTypeGrouping == Tile_Type_Grouping::ALL_TILE_TYPES) ||
						((TileTypeGrouping == Tile_Type_Grouping::LAND_TILE_TYPES) && (Grid.CustomCellData[TileIndex].isWaterTile() != true)) ||
						((TileTypeGrouping == Tile_Type_Grouping::OCEAN_TILE_TYPES) && (Grid.CustomCellData[TileIndex].isWaterTile())))
					{
						Result.push_back(TileIndex);
					}
				}
			}
		}
		
		// ---- left
		{
			for (int j = 0; j < RingSize; j++)
			{
				Row--;
				
				TileIndex = Grid.getCellIndex_ValidateCoordinates(Col, Row);
				if (Grid.isValidCell(TileIndex))
				{
					if ((TileTypeGrouping == Tile_Type_Grouping::ALL_TILE_TYPES) ||
						((TileTypeGrouping == Tile_Type_Grouping::LAND_TILE_TYPES) && (Grid.CustomCellData[TileIndex].isWaterTile() != true)) ||
						((TileTypeGrouping == Tile_Type_Grouping::OCEAN_TILE_TYPES) && (Grid.CustomCellData[TileIndex].isWaterTile())))
					{
						Result.push_back(TileIndex);
					}
				}
			}
		}
		
		if (TileTypeGrouping == Tile_Type_Grouping::OCEAN_TILE_TYPES)
		{
			AreaCacheIndexMap_Water[Center].push_back((int)Result.size());
		}
		else if (TileTypeGrouping == Tile_Type_Grouping::LAND_TILE_TYPES)
		{
			AreaCacheIndexMap_Land[Center].push_back((int)Result.size());
		}
		else
		{
			AreaCacheIndexMap_All[Center].push_back((int)Result.size());
		}
	}
	
	return Result;
}

////////////////////////////////////////////////////////////
