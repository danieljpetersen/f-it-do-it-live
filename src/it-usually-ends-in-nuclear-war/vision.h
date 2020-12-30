#ifndef IUEINW_VISION_H
#define IUEINW_VISION_H

#include <unordered_map>

namespace IUEINW
{
    class IUEINW_Vision : public fi::Plugin_Base
	{
	private:
		std::vector<std::vector<int>> VisionCount; // [0] == TileIndexes, [1] == NationIndexes;  VisionCount of -1 indicates not explored
		std::vector<std::vector<int>> LastTickSeen; // [0] == TileIndexes, [1] == NationIndexes;  VisionCount of -1 indicates not explored

	public:
		void init(int NumberOfNations);
		void revealTile(int TileIndex, int NationIndex);
		void incrementVision(int TileIndex, int NationIndex);
		void decrementVision(int TileIndex, int NationIndex);
		void incrementArea(int CenterTileIndex, int NationIndex, int HalfAreaSize);
		void decrementArea(int CenterTileIndex, int NationIndex, int HalfAreaSize);
		void clearVision(int TileIndex, int NationIndex);
		void clearVision_AllNations(int TileIndex);

		int lastTickSeen(int TileIndex, int NationIndex);
		int lastTickSeen_ignoreCheatMode(int TileIndex, int NationIndex);
		bool canHumanSeeCity(int TileIndex, int TickCityCreated);

		bool hasHumanNationExplored(int TileIndex);
		bool hasHumanNationExplored_IgnoreCheatMode(int TileIndex);
		bool hasHumanNationDiscoveredBottomLeftEdge();
		bool hasHumanNationDiscoveredBottomRightEdge();
		bool hasHumanNationDiscoveredTopLeftEdge();
		bool hasHumanNationDiscoveredTopRightEdge();
		bool hasHumanNationDiscoveredAllEdges();

		bool hasNationExplored(int NationIndex, int TileIndex);
		bool hasNationExplored_IgnoreCheatMode(int NationIndex, int TileIndex);
		bool hasNationExploredProvinceCapital_ByCityId(int NationIndex, gr::Slot_Map_ID CityId);
		bool hasNationExploredProvinceCapital_ByTileIndex(int NationIndex, int TileIndex);

		bool hasVision(int TileIndex);
		bool hasVision_IgnoreCheatMode(int TileIndex);

		bool canAnyoneSee(int TileIndex);
		bool canNationSee(int TileIndex, int NationIndex);
	};

	IUEINW_Vision &getVision();
}

#endif