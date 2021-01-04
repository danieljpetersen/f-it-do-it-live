#include "map_generation.h"
#include "iueinw.h"
#include "nations.h"

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Plugin_Generate_Map::work(const int Event)
{
    getMap().PossibleMapLayouts.initFromJSON();
    getMap().TileTypeProperties.initFromJSON();
    getMap().CurrentMapLayout = getMap().PossibleMapLayouts.getSuggestedMapSize();

    getTiles().init(getMap().CurrentMapLayout.LargeColumnCount, getMap().CurrentMapLayout.LargeRowCount);
    SmallBoard.init(getMap().CurrentMapLayout.getSmallCols(), getMap().CurrentMapLayout.getSmallRows());

    smallBoard_generateLandAndOcean();
    smallBoard_generateTileTypes();

    largeBoard_setTileTypesBasedOnSmallBoard();
    largeBoard_removeBlockinessFromTranslatedMap();
    largeBoard_removeBlockinessFromTranslatedMap();

    getTiles().populateAreaCache();
    largeBoard_blendTiles();

    largeBoard_setTilePropertiesBasedOnCurrentTileType();//todo
    largeBoard_blendTileTypes();
    largeBoard_setTileIntensitiesAkaBonusOutput();
    largeBoard_finalTileTypeAssignment();

    getTiles().determineContinents();
    getTiles().determineBodiesOfWater();
    getTiles().determineLandAndWaterTiles();
    largeBoard_setCoastalTiles();

    if (getTiles().Y_TILE_SLOPE_OFFSET)
    {
        generateHeights();
    }

    getTiles().Grid.updateCenterPositions();

    largeBoard_determineTileShadows();
    TileTypes.clear();

    fi::getPlugins().delayedExecute(EVENT_BUILD_MAP_DRAWABLES);
}