#ifndef IUEINW_PATHFINDING_H
#define IUEINW_PATHFINDING_H

#include "../common/engine.h"
#include "map.h"
#include "path.h"
#include "units.h"

namespace IUEINW
{
	// we should have two pathfinders - one for small board, one for large
	// some paths don't need to be done on the full sized board to perform their function
	// ALTERNATIVELY, do pathfinding from each city tile to every other city tile. when you want to path somewhere, use this hyperlane-like structure to find an approximation first

	////////////////////////////////////////////////////////////

	class IUEINW_Pathfinding_Async_Work_Order
    {
    public:
        IUEINW_Pathfinding_Async_Work_Order() = default;
        IUEINW_Pathfinding_Async_Work_Order(int StartTile, int GoalTile, bool Land, bool Water, bool LookingOnlyOnExplored, int NationIndex, int UnitPrototypeIndex)
        {
            this->StartTile = StartTile;
            this->GoalTile = GoalTile;
            this->Land = Land;
			this->Water = Water;
			this->LookingOnlyOnExplored = LookingOnlyOnExplored;
			this->NationIndex = NationIndex;
			this->UnitPrototypeIndex = UnitPrototypeIndex;
        }

		int StartTile = 0;
		int GoalTile = 0;
		int NationIndex = 0;
		int UnitPrototypeIndex = 0;
		bool Water = false;
		bool Land = false;
		bool LookingOnlyOnExplored = false;

		IUEINW_Path Path;
		atomicwrapper<bool> Completed = atomicwrapper<bool>(false);
    };

	////////////////////////////////////////////////////////////

	class IUEINW_Pathfinder : public fi::AStar_Pathfinder
	{
	public:
		bool isValidTile(int TileIndex) override
		{
			if (getGrid().CustomCellData[TileIndex].isWaterTile())
			{
				return false;
			}

			return true;
		}

		bool isExplored(int TileIndex) override
		{
			return false;
		}

	protected:
		int determineGScore(fi::Data_AStar_Find_Path &Data, fi::AStar_Pathfinding_Node &Location, int ParentGScore, int RelationToParent) override
		{
			auto GScore = fi::AStar_Pathfinder::determineGScore(Data, Location, ParentGScore, RelationToParent);

			GScore += getGrid().CustomCellData[Location.Tile].calculateLossFromAttrition(Data.NationIndex) * 10;

			//if (Grid->CustomCellData[Location.Tile].HasRoad)
			//{
			//	if ((Unit == nullptr) || (Unit->TickUpdateInterval > 1))
			//	{
			//		Increase -= 10;
			//	}
			//}

			Location.GScore = GScore;
			return GScore;
		}
	};

	////////////////////////////////////////////////////////////

	class IUEINW_Plugin_Pathfinding : public fi::Plugin_Base, public fi::Work_Queue<std::shared_ptr<IUEINW_Pathfinding_Async_Work_Order>>
    {
    public:
        void onEnableChange(bool Enabling) override
		{
        	if (Enabling)
			{
				Pathfinder.init(&getGrid());
			}
		}

		// ----

		IUEINW_Path findPath(int StartTile, int GoalTile, bool Land, bool Water, bool LookingOnlyOnExplored, int NationIndex, int UnitPrototypeIndex)
		{
        	IUEINW_Path Path;
			Path.Path = std::shared_ptr<std::deque<int>>(Pathfinder.findPath(StartTile, GoalTile, Land, Water, LookingOnlyOnExplored, NationIndex, UnitPrototypeIndex));
			Path.UnitID = fi::Slot_Map_ID::null();
			return std::move(Path);
		}

		// ----

		IUEINW_Path findPath(IUEINW::IUEINW_Unit *Unit, int GoalTile, bool LookingOnlyOnExplored)
		{
			bool Land = true;
			bool Water = false;
			IUEINW_Path Path;
			Path.Path = std::shared_ptr<std::deque<int>>(Pathfinder.findPath(Unit->TileIndex, GoalTile, Land, Water, LookingOnlyOnExplored, Unit->NationIndex, Unit->PrototypeIndex));
			Path.UnitID = Unit->getSlotMapID();
			return std::move(Path);
		}

	private:
    	IUEINW_Pathfinder Pathfinder;

		// ----

		void asyncWork(std::shared_ptr<IUEINW_Pathfinding_Async_Work_Order> WorkOrder) override
		{
			WorkOrder->Path = findPath(WorkOrder->StartTile, WorkOrder->GoalTile, WorkOrder->Land, WorkOrder->Water, WorkOrder->LookingOnlyOnExplored, WorkOrder->NationIndex, WorkOrder->UnitPrototypeIndex);
			WorkOrder->Completed._a.store(true);
		}
	};

    ////////////////////////////////////////////////////////////

    IUEINW_Plugin_Pathfinding &getPathfinding();
}

#endif