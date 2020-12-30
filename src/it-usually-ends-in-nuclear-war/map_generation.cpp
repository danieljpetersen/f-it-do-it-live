#include "map_generation.h"
#include "iueinw.h"
#include "nations.h"

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Map_Generator::onEnableChange(bool Enabling)
{
    //IUEINW_Map_Layout MapSize;
    //MapSize.init("", )
    if (Enabling)
    {
        fi::getRandom().seed();
        fi::getEngine().setCursorStyle(sf::StandardCursor::WAIT);
        //fi::getEngine().resetECS();

        getMap().onMapGeneration();

        this->generate();

        //getProvinces().onMapGeneration();
        getNations().onMapGeneration();
        getMap().buildDrawable();
        //getPathfinder().init(getMap().Tiles.size());
        //getUnits().onMapGeneration();

        fi::getPlugins().clearStackThenTransitionToProgramState(getApp().ProgramStateIndex_Core);
        fi::getEngine().setCursorStyle(sf::StandardCursor::NORMAL);
        fi::flagUpdate();
    }
}

////////////////////////////////////////////////////////////

IUEINW::IUEINW_Map_Generator &IUEINW::getMapGenerator()
{
    return getApp().MapGenerator;
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Map_Generator::generate()
{
    this->MapLayout = getMap().CurrentMapLayout;

    CheckIndex = 0;
	TileTypesIndex = 0;

	getTiles().init(MapLayout.LargeColumnCount, MapLayout.LargeRowCount);
	SmallBoard.init(MapLayout.getSmallCols(), MapLayout.getSmallRows());

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

	largeBoard_labelLatitudeZones();

	getTiles().determineContinents();
	getTiles().determineBodiesOfWater();
	getTiles().determineLandAndWaterTiles();
	largeBoard_setCoastalTiles();

	if (getTiles().Y_TILE_SLOPE_OFFSET)
	{
		generateHeights();
	}

	getTiles().Grid.updateCenterPositions();
	getMap().TileTypeProperties.rankTiles();

	largeBoard_determineTileShadows();
	TileTypes.clear();
}