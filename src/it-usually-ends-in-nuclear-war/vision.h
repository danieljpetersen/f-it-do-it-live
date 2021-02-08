#ifndef IUEINW_VISION_H
#define IUEINW_VISION_H

#include <unordered_map>

namespace IUEINW
{
    class IUEINW_Vision
	{
	private:
		std::vector<int> VisionCount; // -1 indicates not explored
		std::vector<int> LastTickSeen; // -1 indicates not explored

	public:
        void init();
        void recalculateVision(); // for cheat mode changing human player; will

		void revealTile(int TileIndex);
		void incrementVision(int TileIndex);
		void decrementVision(int TileIndex);
		void incrementArea(int CenterTileIndex, int HalfAreaSize);
		void decrementArea(int CenterTileIndex, int HalfAreaSize);
		void clearVision(int TileIndex);

		int lastTickSeen(int TileIndex);
		int lastTickSeen_ignoreCheatMode(int TileIndex);

		bool hasExplored(int TileIndex);
		bool hasExplored_IgnoreCheatMode(int TileIndex);
		bool hasVision(int TileIndex);
		bool hasVision_IgnoreCheatMode(int TileIndex);

		std::vector<std::vector<int>> EdgeTilesDiscovered;
		bool hasDiscoveredBottomLeftEdge();
		bool hasDiscoveredBottomRightEdge();
		bool hasDiscoveredTopLeftEdge();
		bool hasDiscoveredTopRightEdge();
		bool hasDiscoveredAllEdges();
		bool hasDiscoveredBottomLeftEdge_ignoreCheats();
		bool hasDiscoveredBottomRightEdge_ignoreCheats();
		bool hasDiscoveredTopLeftEdge_ignoreCheats();
		bool hasDiscoveredTopRightEdge_ignoreCheats();
	};

    IUEINW_Vision &getVision();
}

#endif