#ifndef IUEINW_MAP_GENERATION_H
#define IUEINW_MAP_GENERATION_H

#include <list>
#include <vector>
#include "../common/engine.h"
#include "map_size.h"
#include "map.h"

////////////////////////////////////////////////////////////

const int MAP_GENERATOR_NOT_CHECKED = 0;
const int MAP_GENERATOR_MAKE_LAND = 1;
const int MAP_GENERATOR_MAKE_WATER = 2;

namespace IUEINW
{
    class IUEINW_Map_Generator : public fi::Plugin_Base
    {
    public:
        void onEnableChange(bool Enabling) override;
        void generate();

        IUEINW_Map_Layout MapLayout;

    private:
        IUEINW_Tiles SmallBoard;

        int TileTypesIndex, CheckIndex;
        std::vector<int> TileTypes;
        std::list<int> TilesToCheck;

        // functions are in the order that they are called
        void smallBoard_generateLandAndOcean();
        void smallBoard_addSurroundingToTilesToLookAt(int Index);

        void generateHeights();
        bool canWeRaisePoint(int TopLeftTile, int TopRightTile, int BottomLeftTile, int BottomRightTile);
        void getPointTiles(int Tile, int Point, int &aTile, int &bTile, int &cTile, int &dTile);
        void changePointHeight(int Tile, int Point, int Height);
        bool isPointWater(int Tile);
        bool isPointWater(int Tile, int Point);

        void smallBoard_generateTileTypes();
        void smallBoard_generateTileType(int LowestRow, int HighestRow, int ModifyChance, int ExpandChance, int TileType);
        void smallBoard_generatePlains();
        void smallBoard_generateHill();
        void smallBoard_generateForest();
        void smallBoard_generateDesert();
        void smallBoard_generateIce();

        void largeBoard_setTileTypesBasedOnSmallBoard();
        void largeBoard_removeBlockinessFromTranslatedMap();

        std::vector<std::vector<int>> TileTypeCounts;
        void largeBoard_blendTileTypes();
        void largeBoard_setTilePropertiesBasedOnCurrentTileType();
        void largeBoard_blendTiles();
        void largeBoard_setTileIntensitiesAkaBonusOutput();
        void largeBoard_finalTileTypeAssignment();

        void largeBoard_setCoastalTiles();
        bool largeBoard_isCoastalTile(int Index);

        void largeBoard_labelLatitudeZones();

        void largeBoard_determineTileShadows();
        void largeBoard_determineTileShadow(int TileIndex);
    };

    IUEINW_Map_Generator &getMapGenerator();

////////////////////////////////////////////////////////////

    class Corner_Tile_Indexes
    {
    public:
        std::vector<int> TilesForPoint;
    };
}

#endif