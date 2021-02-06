#ifndef FI_ASTAR_PATFINDER_H
#define FI_ASTAR_PATFINDER_H

/* https://web.archive.org/web/20170704235033/http://www.policyalmanac.org/games/aStarTutorial.htm
1) Add the starting square (or node) to the open list.
2) Repeat the following:
a) Look for the lowest F cost square on the open list. We refer to this as the current square.
b) Switch it to the closed list.
c) For each of the 8 squares adjacent to this current square �

 If it is not walkable or if it is on the closed list, ignore it. Otherwise do the following.

If it isn't on the open list, add it to the open list. Make the current square the parent of this square. Record the F, G, and H costs of the square.
If it is on the open list already, check to see if this path to that square is better, using G cost as the measure. A lower G cost means that this is a better path. If so, change the parent of the square to the current square, and recalculate the G and F scores of the square. If you are keeping your open list sorted by F score, you may need to resort the list to account for the change.

d) Stop when you:

 Add the target square to the closed list, in which case the path has been found (see note below), or
Fail to find the target square, and the open list is empty. In this case, there is no path.

3) Save the path. Working backwards from the target square, go from each square to its parent square until you reach the starting square. That is your path.

*/

////////////////////////////////////////////////////////////

#include <list>
#include <vector>
#include <string>

namespace fi
{
	class AStar_Pathfinding_Node
	{
	public:
		int Tile, Parent;
		int GScore, HScore, FScore;

		bool operator<( const AStar_Pathfinding_Node& val ) const {
			return FScore < val.FScore;
		}
	};

	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////

	struct Data_AStar_Find_Path
	{
		bool GoalOnList, OpenListEmpty;
		int StartTile, GoalTile;
		bool LookingOnlyOnExplored, LookOnWater, LookOnLand;
		int NationIndex = -1;
		int UnitPrototypeIndex = -1;

		std::vector<bool> IsTileOnClosedList, IsTileOnOpenList;
		std::vector<bool> CheckedOrInQueueToCheck;// for unexplored function
		std::list<AStar_Pathfinding_Node> OpenList, ClosedList;
		std::deque<int> *Path;
	};

	////////////////////////////////////////////////////////////

	class AStar_Pathfinder
	{
	protected:
		fi::Grid_Base *Grid;

		void startSearch(fi::Data_AStar_Find_Path &Data);
		void continueSearch(fi::Data_AStar_Find_Path &Data);
		void recordFinalPath(fi::Data_AStar_Find_Path &Data);

		void insertToOpenListSortedByFScore(fi::Data_AStar_Find_Path &Data, AStar_Pathfinding_Node MyNode);
		bool validTileToAddToOpenList(fi::Data_AStar_Find_Path &Data, int Tile);
		bool handleOpenListDuplicates(fi::Data_AStar_Find_Path &Data, AStar_Pathfinding_Node &TileToTest, int ParentGScore, int RelationToParent, int &GSCoreCalculation);

		virtual int determineGScore(fi::Data_AStar_Find_Path &Data, AStar_Pathfinding_Node &Location, int ParentGScore, int RelationToParent);
		int determineHScore(fi::Data_AStar_Find_Path &Data, AStar_Pathfinding_Node &Location);
		inline int determineFScore(fi::Data_AStar_Find_Path &Data, AStar_Pathfinding_Node &Location);

		virtual bool isExplored(int TileIndex) { return false; }
		virtual bool isValidTile(int TileIndex) { return true; }

	public:
		void init(fi::Grid_Base *Grid);

		// note -- caller is responsible for managing the return value lifetime
		std::deque<int> *findPath(int StartTile, int GoalTile, bool Land, bool Water, bool LookingOnlyOnExplored, int NationIndex, int UnitPrototypeIndex);

		int findClosestUnexploredTile(int Tile, int NationIndex, bool SearchLand, bool SearchOcean);
	};
}

#endif