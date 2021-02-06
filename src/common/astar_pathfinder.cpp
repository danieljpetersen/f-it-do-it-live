#include "engine.h"
#include "astar_pathfinder.h"

////////////////////////////////////////////////////////////

void fi::AStar_Pathfinder::init(fi::Grid_Base *Grid)
{
	this->Grid = Grid;
}

////////////////////////////////////////////////////////////

std::deque<int> *fi::AStar_Pathfinder::findPath(int StartTile, int GoalTile, bool Land, bool Water, bool LookingOnlyOnExplored, int NationIndex, int UnitPrototypeIndex)
{
	if (StartTile == GoalTile)
	{
		return new std::deque<int> { GoalTile };
	}

	fi::Data_AStar_Find_Path Data;
	Data.StartTile = StartTile;
	Data.GoalTile = GoalTile;
	Data.LookOnWater = Water;
	Data.LookOnLand = Land;
	Data.LookingOnlyOnExplored = LookingOnlyOnExplored;
	Data.NationIndex = NationIndex;
	Data.UnitPrototypeIndex = UnitPrototypeIndex;
	Data.Path = new std::deque<int>();

	const int NumberOfTiles = Grid->size();
	Data.IsTileOnOpenList.resize(NumberOfTiles, false);
	Data.IsTileOnClosedList.resize(NumberOfTiles, false);
	Data.IsTileOnOpenList.resize(NumberOfTiles, false);
	Data.CheckedOrInQueueToCheck.resize(NumberOfTiles, false);

	this->startSearch(Data);
	this->continueSearch(Data);
	this->recordFinalPath(Data);

	return Data.Path;
}

////////////////////////////////////////////////////////////

void fi::AStar_Pathfinder::startSearch(fi::Data_AStar_Find_Path &Data)
{
	AStar_Pathfinding_Node StartingNode;
	StartingNode.Tile = Data.StartTile;
	StartingNode.Parent = Data.StartTile;

	StartingNode.GScore = 0;
	StartingNode.HScore = determineHScore(Data, StartingNode);
	StartingNode.FScore = StartingNode.HScore;

	Data.OpenList.push_front(StartingNode);
	Data.IsTileOnOpenList[StartingNode.Tile] = true;
}

////////////////////////////////////////////////////////////

void fi::AStar_Pathfinder::continueSearch(fi::Data_AStar_Find_Path &Data)
{
	AStar_Pathfinding_Node NodeToTest;
	int Neighbor = -1, CenterTile = -1, ParentGScore;

	while (Data.OpenList.size() > 0)
	{
		CenterTile = Data.OpenList.front().Tile;
		ParentGScore = Data.OpenList.front().GScore;
		Data.ClosedList.push_back(Data.OpenList.front());
		Data.OpenList.pop_front();

		Data.IsTileOnClosedList[CenterTile] = true;
		Data.IsTileOnOpenList[CenterTile] = false;
		NodeToTest.Parent = CenterTile;

		int CenterRow = Grid->CommonCellData[CenterTile].Row;
		int CenterCol = Grid->CommonCellData[CenterTile].Column;

		for (unsigned int i = 0; i < Grid->Neighbors[CenterTile].size(); i++)
		{
			Neighbor = Grid->Neighbors[CenterTile][i];
			if (this->validTileToAddToOpenList(Data, Neighbor))
			{
				if (Data.IsTileOnClosedList[Neighbor] != true)
				{
					NodeToTest.Tile = Neighbor;

					int NeighborRow = Grid->CommonCellData[Neighbor].Row;
					int NeighborCol = Grid->CommonCellData[Neighbor].Column;
					int RowColDifference = abs(abs(CenterCol - NeighborCol) - abs(CenterRow - NeighborRow)); // todo -- optimize...?

					int GScore=-1;
					if (handleOpenListDuplicates(Data, NodeToTest, ParentGScore, RowColDifference, GScore) != true)
					{
						if (GScore != -1) // this micro optimization is prob slower than doing calculating gscore twice
						{
							NodeToTest.GScore = GScore;
						}
						else
						{
							NodeToTest.GScore = this->determineGScore(Data, NodeToTest, ParentGScore, RowColDifference);
						}
						NodeToTest.HScore = this->determineHScore(Data, NodeToTest);
						NodeToTest.FScore = this->determineFScore(Data, NodeToTest);

						insertToOpenListSortedByFScore(Data, NodeToTest);
						if (Neighbor == Data.GoalTile)
						{
							Data.ClosedList.push_front(NodeToTest);
							return;
						}
					}
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////

void fi::AStar_Pathfinder::insertToOpenListSortedByFScore(fi::Data_AStar_Find_Path &Data, AStar_Pathfinding_Node MyNode)
{
	// TODO -- should figure out which one is more efficient
	Data.IsTileOnOpenList[MyNode.Tile] = true;
//	OpenList.push_back(MyNode);
//	OpenList.sort();

	for (std::list<AStar_Pathfinding_Node>::iterator iterator = Data.OpenList.begin(), end = Data.OpenList.end(); iterator != end; ++iterator)
	{
		if ((*iterator).FScore > MyNode.FScore)
		{
			Data.OpenList.insert(iterator, MyNode);
			return;
		}
	}

	Data.OpenList.push_back(MyNode);
}

////////////////////////////////////////////////////////////

int fi::AStar_Pathfinder::determineGScore(fi::Data_AStar_Find_Path &Data, AStar_Pathfinding_Node &Location, int ParentGScore, int RelationToParent)
{
	//terrain penalties can be applied here (ie:  GScore += WATER_TERRAIN_PENALTY)
	int Increase;
	if (RelationToParent == 0)
	{
		Increase = 14; // non-diagonal
	}
	else
	{
		Increase = 10; // diagonal
	}

	Location.GScore = ParentGScore + Increase;
	return Location.GScore;
}

////////////////////////////////////////////////////////////

int fi::AStar_Pathfinder::determineHScore(fi::Data_AStar_Find_Path &Data, AStar_Pathfinding_Node &Location)
{
	Location.HScore = Grid->getDistanceBetweenTiles(Location.Tile, Data.GoalTile) * 10;
	return Location.HScore;
}

////////////////////////////////////////////////////////////

inline int fi::AStar_Pathfinder::determineFScore(fi::Data_AStar_Find_Path &Data, AStar_Pathfinding_Node &Location)
{
	return Location.GScore + Location.HScore;
}

////////////////////////////////////////////////////////////

bool fi::AStar_Pathfinder::validTileToAddToOpenList(fi::Data_AStar_Find_Path &Data, int Tile)
{
	if (Grid->isValidTile(Tile) != true)
		return false;

	//tiles with units of other players, or maybe if we have an unmovable mountain tile here would return false
	if (Data.LookOnWater == false)
	{
		if (this->isValidTile(Tile) != true)
		{
			return false;
		}
	}

	return true;
}

////////////////////////////////////////////////////////////

bool fi::AStar_Pathfinder::handleOpenListDuplicates(fi::Data_AStar_Find_Path &Data, AStar_Pathfinding_Node &NodeToTest, int ParentGScore, int RelationToParent, int &GSCoreCalculation)
{
	if (Data.IsTileOnOpenList[NodeToTest.Tile] == true)
	{
		NodeToTest.GScore = determineGScore(Data, NodeToTest, ParentGScore, RelationToParent);
		GSCoreCalculation = NodeToTest.GScore;

		std::list<AStar_Pathfinding_Node>::iterator iter = Data.OpenList.begin();
		while (iter != Data.OpenList.end())
		{
			if ((*iter).Tile == NodeToTest.Tile)
			{
				if ((*iter).GScore > NodeToTest.GScore)
				{
					NodeToTest.HScore = (*iter).HScore;
					NodeToTest.FScore = determineFScore(Data, NodeToTest);

					(*iter).Parent = NodeToTest.Parent;
					(*iter).GScore = NodeToTest.GScore;
					(*iter).FScore = NodeToTest.FScore;

					insertToOpenListSortedByFScore(Data, (*iter));
					Data.OpenList.erase(iter);
				}
				return true;
			}
			iter++;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////

void fi::AStar_Pathfinder::recordFinalPath(fi::Data_AStar_Find_Path &Data)
{
	for (std::list<AStar_Pathfinding_Node>::const_iterator iter = Data.ClosedList.begin(), end = Data.ClosedList.end(); iter != end; ++iter)
	{
		Data.IsTileOnClosedList[(*iter).Tile] = false;
	}

	Data.Path->push_back(Data.ClosedList.front().Parent);
	Data.Path->push_back(Data.ClosedList.front().Tile);
	Data.ClosedList.pop_front();

	bool Increment = true;
	bool ReachedStart = false;
	std::list<AStar_Pathfinding_Node>::iterator iter = Data.ClosedList.begin();
	while ((iter != Data.ClosedList.end()) && (ReachedStart == false))
	{
		if ((*iter).Tile == Data.Path->front())
		{
			Data.Path->push_front((*iter).Parent);
			if (Data.Path->front() == Data.StartTile)
				ReachedStart = true;

			Data.ClosedList.erase(iter);
			iter = Data.ClosedList.begin();
			Increment = false;
		}
		if (Increment == true)
			iter++;
		Increment = true;
	}

	Data.Path->pop_front();
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

int fi::AStar_Pathfinder::findClosestUnexploredTile(int TileIndex, int NationIndex, bool SearchLand, bool SearchOcean)
{
	//Add Tile to ToCheck list
	//Loop through ToCheck
	//Mark first tile as checked
	//for each neighbor of first tile in ToCheck
	//if Land (and we want land) || Ocean (and we want ocean)
	//if Not checked yet
	//add tile to ToChecked
	//If unexplored
	//ba-zam

	//bool Valid;
	//LookingOnlyOnExplored = false;
	//int StartTileIndex = TileIndex, Neighbor;
	//this->Unit = Unit;
	//
	//for (int i = 0; i < CheckedOrInQueueToCheck.size(); i++)
	//{
	//	CheckedOrInQueueToCheck[i] = false;
	//}
	//
	//std::deque<int> ToCheck;
	//ToCheck.push_back(TileIndex);
	//while (ToCheck.size() > 0)
	//{
	//	TileIndex = ToCheck.front();
	//	ToCheck.pop_front();
	//	CheckedOrInQueueToCheck[TileIndex] = true;
	//
	//	std::vector<int> NeighborOrder;
	//	for (int i = 0; i < Grid->Neighbors[TileIndex].size(); i++)
	//	{
	//		NeighborOrder.push_back(i);
	//	}
	//
	//	gr::vectorShuffle(&NeighborOrder, 4);
	//
	//	for (unsigned int i = 0; i < Grid->Neighbors[TileIndex].size(); i++)
	//	{
	//		Valid = false;
	//		Neighbor = Grid->Neighbors[TileIndex][NeighborOrder[i]];
	//		if (CheckedOrInQueueToCheck[Neighbor] != true)
	//		{
	//			CheckedOrInQueueToCheck[Neighbor] = true;
	//
	//			//if ((Grid->CustomCellData[Neighbor].TileType == OCEAN) && (Grid->CustomCellData[Neighbor].TileType != COASTAL_OCEAN))
	//			if (Grid->CustomCellData[Neighbor].TileType == OCEAN)
	//			{
	//				if (SearchOcean)
	//				{
	//					Valid = true;
	//				}
	//			}
	//			else
	//			{
	//				if (SearchLand)
	//				{
	//					Valid = true;
	//				}
	//			}
	//
	//			if (Valid)
	//			{
	//				ToCheck.push_back(Neighbor);
	//
	//				if ((Grid->CustomCellData[Neighbor].TileType == COASTAL_OCEAN) && (SearchOcean != true))
	//				{
	//					ToCheck.pop_back();
	//				}
	//
	//				if (getVision().hasNationExplored_IgnoreCheatMode(NationIndex, Neighbor) != true)
	//				{
	//					int DestinationTile = TileIndex;
	//
	//					// If we're not searching ocean, and the unexplored tile is coastal, we need to check all of its neighbors for land tile
	//					if ((SearchOcean != true) && (Grid->CustomCellData[Neighbor].TileType == COASTAL_OCEAN))
	//					{
	//						for (unsigned int j = 0; j < Grid->Neighbors[Neighbor].size(); j++)
	//						{
	//							int OtherTileIndex = Grid->Neighbors[Neighbor][j];
	//							if (Grid->CustomCellData[StartTileIndex].ContinentIndex == Grid->CustomCellData[OtherTileIndex].ContinentIndex)
	//							{
	//								DestinationTile = OtherTileIndex;
	//								break;
	//							}
	//						}
	//					}
	//
	//					//mother of god...  we've found it!!1!11!!!
	//					this->findLandPath(StartTileIndex, DestinationTile, true, Unit);
	//					return DestinationTile;
	//				}
	//
	//				// todo
	//				//if (getTiles()[Neighbor].HasGoodyHut) // this should go below the vision check in an else if (..) statement, but for some reason that crashes
	//				//{
	//				//	this->findLandPath(StartTileIndex, Neighbor, true, Unit);
	//				//	return Neighbor;
	//				//}
	//			}
	//		}
	//	}
	//}

	return -1;
}


////////////////////////////////////////////////////////////

