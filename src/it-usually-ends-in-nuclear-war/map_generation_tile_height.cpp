#include "iueinw.h"
#include "map_generation.h"
#include "nations.h"

void IUEINW::IUEINW_Map_Generator::generateHeights()
{
	for (int i = 0; i < getTiles().Grid.size(); i++)
	{
		getTiles().Grid.CustomCellData[i].Height[0] = 0;
		getTiles().Grid.CustomCellData[i].Height[1] = 0;
		getTiles().Grid.CustomCellData[i].Height[2] = 0;
		getTiles().Grid.CustomCellData[i].Height[3] = 0;
	}

	std::vector<Corner_Tile_Indexes> Corner;
	Corner.resize(0);
	int CornerIndex = 0;
	const int TopLeft = TOPLEFT, TopRight = TOPRIGHT, BottomLeft = BOTTOMLEFT, BottomRight = BOTTOMRIGHT;

	// basically grabbing the tiles connected to every left point (or top left if we're square grid)
	for (int i = 0; i < getTiles().Grid.size(); i++)	//init corners with tile number
	{
		int Row = getTiles().Grid.CommonCellData[i].Row;
		int Col = getTiles().Grid.CommonCellData[i].Column;
		if ((getTiles().Grid.CommonCellData[i].Edge == NO_EDGE) || (getTiles().Grid.CommonCellData[i].Edge == RIGHT_EDGE))
		{
			// 0 and 1 are fucked -- make sure the topright corner tile is not being set to RIGHT_EDGE
			Corner.resize(CornerIndex + 1);
			Corner[CornerIndex].TilesForPoint.resize(4);
			Corner[CornerIndex].TilesForPoint[TopLeft] = getTiles().Grid.getCellIndex_ValidateCoordinates(Col-1, Row-1);
			Corner[CornerIndex].TilesForPoint[TopRight] = getTiles().Grid.getCellIndex_ValidateCoordinates(Col, Row-1);
			Corner[CornerIndex].TilesForPoint[BottomLeft] = getTiles().Grid.getCellIndex_ValidateCoordinates(Col-1, Row);
			Corner[CornerIndex].TilesForPoint[BottomRight] = i;

			CornerIndex++;
		}
	}

	std::list<Corner_Tile_Indexes*> UnlockedList;
	std::list<Corner_Tile_Indexes*> LockedList;

	bool AddToLocked = false;
	const int NumberOfCorners = Corner.size();
	//two lists, locked list and unlocked list.  water tiles get locked
	for (int i = 0; i < NumberOfCorners; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (getTiles().Grid.CustomCellData[Corner[i].TilesForPoint[j]].isWaterTile())
			{
				AddToLocked = true;
			}
		}

		if (AddToLocked == true)
		{
			LockedList.push_back(&Corner[i]);
		}
		else
		{
			UnlockedList.push_back(&Corner[i]);
		}

		AddToLocked = false;
	}

	std::list<Corner_Tile_Indexes*> UnlockedListSaved = UnlockedList;	//for height smoothing at the end

	//iterate through the unlocked list,
	std::list<Corner_Tile_Indexes*>::iterator iter = UnlockedList.begin();
	const int PercentChanceToLock = fi::getRandom().i(1, 3);
	bool Increment = false;
	while (UnlockedList.size() > 0)
	{
		Increment = false;
		if (fi::getRandom().i(0, 100) > PercentChanceToLock)
		{
			Increment = true;
			int CurrentHeight = getTiles().Grid.CustomCellData[(*iter)->TilesForPoint[0]].Height[RIGHT];

			if (canWeRaisePoint((*iter)->TilesForPoint[0], (*iter)->TilesForPoint[1], (*iter)->TilesForPoint[2], (*iter)->TilesForPoint[3]) == true)
			{
				//thre wasn't enough tendancy for tiles to be at sea level
				/*
				//but this didnt seem to do anything so nevermind
				if (CurrentHeight == 0)
				{
					if (Rand.App.Rand->IRandom(0, 100) >= 80)
						changePointHeight((*iter)->TilesForPoint[0], RIGHT, ++CurrentHeight);
				}
				else*/
				changePointHeight((*iter)->TilesForPoint[0], RIGHT, ++CurrentHeight);

			}
			else
			{
				Increment = false;
			}
		}

		if (Increment == true)
		{
			iter++;
		}
		else
		{
			UnlockedList.erase(iter++);
		}

		if (iter == UnlockedList.end())
		{
			iter = UnlockedList.begin();
		}
	}

	//smooth out terrain -- if 5 or more neighboring points are +1 higher than the current point, flag it to be increased
	std::vector<bool> IncreasePoint;
	IncreasePoint.resize(UnlockedListSaved.size());
	iter = UnlockedListSaved.begin();
	int NeighborsOneHigher = 0, MiddleHeight = 0;
	for (unsigned int i = 0; i < UnlockedListSaved.size(); i++)
	{
		MiddleHeight = getTiles().Grid.CustomCellData[(*iter)->TilesForPoint[TopLeft]].Height[RIGHT];

		const int OneHigher = MiddleHeight + 1;
		NeighborsOneHigher = 0;

		if (getTiles().Grid.CustomCellData[(*iter)->TilesForPoint[TopLeft]].Height[LEFT] == OneHigher)
			NeighborsOneHigher++;
		if (getTiles().Grid.CustomCellData[(*iter)->TilesForPoint[TopLeft]].Height[BACK] == OneHigher)
			NeighborsOneHigher++;
		if (getTiles().Grid.CustomCellData[(*iter)->TilesForPoint[TopLeft]].Height[FRONT] == OneHigher)
			NeighborsOneHigher++;

		if (getTiles().Grid.CustomCellData[(*iter)->TilesForPoint[TopRight]].Height[LEFT] == OneHigher)
			NeighborsOneHigher++;
		if (getTiles().Grid.CustomCellData[(*iter)->TilesForPoint[TopRight]].Height[BACK] == OneHigher)
			NeighborsOneHigher++;
		if (getTiles().Grid.CustomCellData[(*iter)->TilesForPoint[TopRight]].Height[RIGHT] == OneHigher)
			NeighborsOneHigher++;


		if (getTiles().Grid.CustomCellData[(*iter)->TilesForPoint[BottomLeft]].Height[LEFT] == OneHigher)
			NeighborsOneHigher++;
		if (getTiles().Grid.CustomCellData[(*iter)->TilesForPoint[BottomLeft]].Height[FRONT] == OneHigher)
			NeighborsOneHigher++;
		if (getTiles().Grid.CustomCellData[(*iter)->TilesForPoint[BottomLeft]].Height[RIGHT] == OneHigher)
			NeighborsOneHigher++;

		if (getTiles().Grid.CustomCellData[(*iter)->TilesForPoint[BottomRight]].Height[BACK] == OneHigher)
			NeighborsOneHigher++;
		if (getTiles().Grid.CustomCellData[(*iter)->TilesForPoint[BottomRight]].Height[FRONT] == OneHigher)
			NeighborsOneHigher++;
		if (getTiles().Grid.CustomCellData[(*iter)->TilesForPoint[BottomRight]].Height[RIGHT] == OneHigher)
			NeighborsOneHigher++;

		if (NeighborsOneHigher >= 5)
			IncreasePoint[i] = true;
		else
			IncreasePoint[i] = false;

		iter++;
	}

	iter = UnlockedListSaved.begin();
	for (unsigned int i = 0; i < IncreasePoint.size(); i++)
	{
		if (IncreasePoint[i] == true)
		{
			if (fi::getRandom().i(0, 100) >= 40)
			{
				int NewHeight = getTiles().Grid.CustomCellData[(*iter)->TilesForPoint[TopLeft]].Height[RIGHT] + 1;
				changePointHeight((*iter)->TilesForPoint[TopLeft], RIGHT, NewHeight);
			}
		}
		iter++;
	}

	getTiles().offsetTileCoordinatesFromHeight();
}

//==================================	

bool IUEINW::IUEINW_Map_Generator::canWeRaisePoint(int TopLeftTile, int TopRightTile, int BottomLeftTile, int BottomRightTile)
{
	//find lowest neighboring point
	int LowestPoint = 100, MiddlePoint = RIGHT, CurrentTile = TopLeftTile;
	const int TilesPerPoint = 4;
	const int PointsPerTile = 4;
	for (int TileIndex = 0; TileIndex < TilesPerPoint; TileIndex++)
	{
		for (int PointIndex = 0; PointIndex < PointsPerTile; PointIndex++)
		{
			if (PointIndex != MiddlePoint)
			{
				if (getTiles().Grid.CustomCellData[CurrentTile].Height[PointIndex] < LowestPoint)
					LowestPoint = getTiles().Grid.CustomCellData[CurrentTile].Height[PointIndex];
			}
		}

		if (TileIndex == 0)
		{
			CurrentTile = TopRightTile;
			MiddlePoint = FRONT;
		}
		if (TileIndex == 1)
		{
			CurrentTile = BottomLeftTile;
			MiddlePoint = BACK;
		}
		if (TileIndex == 2)
		{
			CurrentTile = BottomRightTile;
			MiddlePoint = LEFT;
		}
	}

	//if lowest neighbor is +/- whatever of the new point height
	int NewPotentialHeight = getTiles().Grid.CustomCellData[TopLeftTile].Height[RIGHT] + 1;
	int Range = 1;
	if (LowestPoint + Range >= NewPotentialHeight)
		return true;

	return false;
}

//==================================

void IUEINW::IUEINW_Map_Generator::getPointTiles(int Tile, int Point, int &aTile, int &bTile, int &cTile, int &dTile)
{
	aTile = Tile;

	bTile = -1;
	cTile = -1;
	dTile = -1;

	int Row = getTiles().Grid.CommonCellData[Tile].Row;
	int Col = getTiles().Grid.CommonCellData[Tile].Column;
	int Neighbor = 0;
	if (Point == LEFT)
	{
		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col-1, Row);
		if (getTiles().Grid.isValidTile(Neighbor))
		{
			bTile = Neighbor;
		}

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col-1, Row-1);
		if (getTiles().Grid.isValidTile(Neighbor))
			cTile = Neighbor;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col, Row-1);
		if (getTiles().Grid.isValidTile(Neighbor))
			dTile = Neighbor;
	}

	if (Point == RIGHT)
	{
		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col, Row+1);
		if (getTiles().Grid.isValidTile(Neighbor))
			bTile = Neighbor;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col+1, Row+1);
		if (getTiles().Grid.isValidTile(Neighbor))
			cTile = Neighbor;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col+1, Row);
		if (getTiles().Grid.isValidTile(Neighbor))
			dTile = Neighbor;
	}

	if (Point == FRONT)
	{
		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col-1, Row);
		if (getTiles().Grid.isValidTile(Neighbor))
			bTile = Neighbor;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col-1, Row+1);
		if (getTiles().Grid.isValidTile(Neighbor))
			cTile = Neighbor;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col, Row+1);
		if (getTiles().Grid.isValidTile(Neighbor))
			dTile = Neighbor;
	}

	if (Point == BACK)
	{
		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col, Row-1);
		if (getTiles().Grid.isValidTile(Neighbor))
			bTile = Neighbor;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col+1, Row-1);
		if (getTiles().Grid.isValidTile(Neighbor))
			cTile = Neighbor;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col+1, Row);
		if (getTiles().Grid.isValidTile(Neighbor))
			dTile = Neighbor;
	}
}

//==================================

void IUEINW::IUEINW_Map_Generator::changePointHeight(int Tile, int Point, int Height)
{
	getTiles().Grid.CustomCellData[Tile].Height[Point] = Height;

	int Row = getTiles().Grid.CommonCellData[Tile].Row;
	int Col = getTiles().Grid.CommonCellData[Tile].Column;

	int Neighbor = 0;
	if (Point == LEFT)
	{
		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col-1, Row);
		if (getTiles().Grid.isValidTile(Neighbor))
			getTiles().Grid.CustomCellData[Neighbor].Height[BACK] = Height;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col-1, Row-1);
		if (getTiles().Grid.isValidTile(Neighbor))
			getTiles().Grid.CustomCellData[Neighbor].Height[RIGHT] = Height;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col, Row-1);
		if (getTiles().Grid.isValidTile(Neighbor))
			getTiles().Grid.CustomCellData[Neighbor].Height[FRONT] = Height;
	}

	if (Point == RIGHT)
	{
		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col, Row+1);
		if (getTiles().Grid.isValidTile(Neighbor))
			getTiles().Grid.CustomCellData[Neighbor].Height[BACK] = Height;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col+1, Row+1);
		if (getTiles().Grid.isValidTile(Neighbor))
			getTiles().Grid.CustomCellData[Neighbor].Height[LEFT] = Height;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col+1, Row);
		if (getTiles().Grid.isValidTile(Neighbor))
			getTiles().Grid.CustomCellData[Neighbor].Height[FRONT] = Height;
	}

	if (Point == FRONT)
	{
		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col-1, Row);
		if (getTiles().Grid.isValidTile(Neighbor))
			getTiles().Grid.CustomCellData[Neighbor].Height[RIGHT] = Height;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col-1, Row+1);
		if (getTiles().Grid.isValidTile(Neighbor))
			getTiles().Grid.CustomCellData[Neighbor].Height[BACK] = Height;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col, Row+1);
		if (getTiles().Grid.isValidTile(Neighbor))
			getTiles().Grid.CustomCellData[Neighbor].Height[LEFT] = Height;
	}

	if (Point == BACK)
	{
		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col, Row-1);
		if (getTiles().Grid.isValidTile(Neighbor))
			getTiles().Grid.CustomCellData[Neighbor].Height[RIGHT] = Height;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col+1, Row-1);
		if (getTiles().Grid.isValidTile(Neighbor))
			getTiles().Grid.CustomCellData[Neighbor].Height[FRONT] = Height;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col+1, Row);
		if (getTiles().Grid.isValidTile(Neighbor))
			getTiles().Grid.CustomCellData[Neighbor].Height[LEFT] = Height;
	}
}

//==================================

bool IUEINW::IUEINW_Map_Generator::isPointWater(int Tile)
{
	if (getTiles().Grid.CustomCellData[Tile].isWaterTile())
		return true;

	int Neighbor;
	for (int i = 0; i < getTiles().Grid.Neighbors[Tile].size(); i++)
	{
		Neighbor = getTiles().Grid.Neighbors[Tile][i];
		if (getTiles().Grid.CustomCellData[Neighbor].isWaterTile())
			return true;
	}

	return false;
}

//==================================

bool IUEINW::IUEINW_Map_Generator::isPointWater(int Tile, int Point)
{
	if (getTiles().Grid.CustomCellData[Tile].isWaterTile())
		return true;

	int Row = getTiles().Grid.CommonCellData[Tile].Row;
	int Col = getTiles().Grid.CommonCellData[Tile].Column;
	int Neighbor = 0;

	if (Point == LEFT)
	{
		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col-1, Row);
		if (getTiles().Grid.isValidTile(Neighbor))
			if (getTiles().Grid.CustomCellData[Neighbor].isWaterTile())
				return true;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col-1, Row-1);
		if (getTiles().Grid.isValidTile(Neighbor))
			if (getTiles().Grid.CustomCellData[Neighbor].isWaterTile())
				return true;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col, Row-1);
		if (getTiles().Grid.isValidTile(Neighbor))
			if (getTiles().Grid.CustomCellData[Neighbor].isWaterTile())
				return true;
	}

	if (Point == RIGHT)
	{
		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col, Row+1);
		if (getTiles().Grid.isValidTile(Neighbor))
			if (getTiles().Grid.CustomCellData[Neighbor].isWaterTile())
				return true;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col+1, Row+1);
		if (getTiles().Grid.isValidTile(Neighbor))
			if (getTiles().Grid.CustomCellData[Neighbor].isWaterTile())
				return true;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col+1, Row);
		if (getTiles().Grid.isValidTile(Neighbor))
			if (getTiles().Grid.CustomCellData[Neighbor].isWaterTile())
				return true;
	}

	if (Point == FRONT)
	{
		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col-1, Row);
		if (getTiles().Grid.isValidTile(Neighbor))
			if (getTiles().Grid.CustomCellData[Neighbor].isWaterTile())
				return true;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col-1, Row+1);
		if (getTiles().Grid.isValidTile(Neighbor))
			if (getTiles().Grid.CustomCellData[Neighbor].isWaterTile())
				return true;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col, Row+1);
		if (getTiles().Grid.isValidTile(Neighbor))
			if (getTiles().Grid.CustomCellData[Neighbor].isWaterTile())
				return true;
	}

	if (Point == BACK)
	{
		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col, Row-1);
		if (getTiles().Grid.isValidTile(Neighbor))
			if (getTiles().Grid.CustomCellData[Neighbor].isWaterTile())
				return true;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col+1, Row-1);
		if (getTiles().Grid.isValidTile(Neighbor))
			if (getTiles().Grid.CustomCellData[Neighbor].isWaterTile())
				return true;

		Neighbor = getTiles().Grid.getCellIndex_ValidateCoordinates(Col+1, Row);
		if (getTiles().Grid.isValidTile(Neighbor))
			if (getTiles().Grid.CustomCellData[Neighbor].isWaterTile())
				return true;
	}

	return false;
}

