#ifndef IUEINW_TRANSPARENCY_TRACKER_H_H
#define IUEINW_TRANSPARENCY_TRACKER_H_H

#include <SFML/System.hpp>
#include <vector>
#include <list>
#include "../common/engine.h"
#include "transparency_tracker.h"

namespace IUEINW
{
    class Tile_Transparency_Tracker : public IUEINW_Transparency_Tracker
    {
    public:
        int AssociatedTile;
    };

    ////////////////////////////////////////////////////////////

    class IUEINW_Plugin_Mouse_Tile_Tracker : public fi::Plugin_Base
    {
    private:
        void findMouseTileIndex();

        void updateTileTransparenciesList();

        void updateMouseHighlightingTiles();

    public:
        void work(const int Event) override;

        int MouseTileIndex =-1;
        int PreviouslyMousedTile = -1;
        bool IsMouseActuallyInsideTile = false;
        std::vector<int> MouseHighlightingTiles;
        std::list<Tile_Transparency_Tracker> TileTransparencies;
    };

    IUEINW_Plugin_Mouse_Tile_Tracker &getMouseTileTracker();
}
#endif