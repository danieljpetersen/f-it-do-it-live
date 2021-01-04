#include "nations.h"
#include "map.h"
#include "color_schemes.h"
#include "vision.h"
#include "cities.h"
#include "drawable_map.h"

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Plugin_Init_Nations::work(const int Event)
{
    getNations().init();
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Nations::init()
{
	auto SpawnTiles = getCities().getSpawnLocations_ReturnsTileIndexes(getMap().CurrentMapLayout.SuggestedNumberOfNations, getMap().CurrentMapLayout.MinimumNationSpawnDistance);

    // actual number of nations can be lower if there aren't enough cities on the map to go around.
    const int NumberOfNations = (int)SpawnTiles.size();
    getMap().CurrentMapLayout.StartingNumberOfNations = NumberOfNations;
    getVision().init(NumberOfNations);

    Nations.clear();
	for (int NationIndex = 0; NationIndex < NumberOfNations; NationIndex++)
	{
		IUEINW::IUEINW_Nation Nation;

		Nation.NationIndex = NationIndex;
		Nation.Name = "";
		Nation.Color = getColorSchemes().getNationColor(NationIndex);
		Nation.IsAlive = true;

		Nation.EdgeTilesDiscovered.clear();
		Nation.EdgeTilesDiscovered.resize(4);
		Nation.EdgeTilesDiscovered[0].clear();
		Nation.EdgeTilesDiscovered[1].clear();
		Nation.EdgeTilesDiscovered[2].clear();
		Nation.EdgeTilesDiscovered[3].clear();
		Nation.TotalNumberOfUnitsInitialized = 0;
		Nation.Cities.clear();
        Nation.Units.clear();

		Nations.push_back(Nation);

		int i = fi::getRandom().i(0, (int)SpawnTiles.size()-1);
        int CityIndex = getTiles()[SpawnTiles[i]].CityIndex;
        SpawnTiles[i] = SpawnTiles.back();
        SpawnTiles.pop_back();

		getCities().setCity(CityIndex, NationIndex);
	}



	 //spawn points

	// json GameplayJSON = gr::jsonConfig["gameplay"].get<json>();
	// json UnitsJSON = GameplayJSON["units"].get<json>();
	// UNIT_CAP_PER_NATION = UnitsJSON["unit-cap-per-nation"].get<json>();
	// auto CategoryNations = UnitsJSON["starting-units"].get<json>();
	//
	// if (CategoryNations.size())
	// {
	// 	for (int i = 0; i < Nations.size()-1; i++) // - 1 due to last nation in array being barbarian
	// 	{
	// 		gr::Slot_Map_ID CityId = Nations[i].Cities[0];
	// 		IUEINW_Base_Entity* City = App.Cities.get(CityId);
	// 		int TileIndex = City->TileIndex;
	// 		Nations[i].setName(City->Name);
	//
	// 		for (json::iterator CategoryIterator = CategoryNations.begin(); CategoryIterator != CategoryNations.end(); ++CategoryIterator)
	// 		{
	// 			App.Units.spawnUnit_noRequirements(i, CityId, TileIndex, (*CategoryIterator));
	// 		}
	// 	}
	// }
	//
	// int CenterTile = App.Cities.get(Nations[HumanNationIndex].Cities.front())->TileIndex;
	// App.Camera.setCenter(IUEINW_Tiles::instance().Grid.CommonCellData[CenterTile].Center);


	HumanNationIndex = 0;
	Human = &Nations[HumanNationIndex];

	// ---- set camera position
    {
        int TileIndex = getCities()[Human->Cities[0]].TileIndex;
        fi::getCanvasWorld().setCenter(getGrid().CommonCellData[TileIndex].getBack2f());
    }
}

////////////////////////////////////////////////////////////

IUEINW::IUEINW_Nation &IUEINW::IUEINW_Nations::getRandomNation()
{
    return Nations[fi::getRandom().i(0, (int)Nations.size()-1)];
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Nations::changeHumanPlayer(int NationIndex)
{
	// todo
	//if ((isValidNation(NewHumanNationIndex) == true) && (Nations[NewHumanNationIndex].getIsAlive() == true))
	{
		//App.Units.deselectUnits();
		//HumanNationIndex = NewHumanNationIndex;
		//Human = &Objects[HumanNationIndex];

		//sf::Vector2f Center = IUEINW_Tiles::instance().getCenterOfTile(App.Cities.Objects[Objects[HumanNationIndex].Cities.front()].getTileIndex());
		//App.Camera.setCenter(Center);
		//App.Drawable_Layers.DrawableMap.update();
		//App.Drawable_Layers.DrawableMap.updateMapEdges();

		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Nations::changeHumanPlayerToNextNation()
{
	// todo
	//int NewIndex = HumanNationIndex + 1;
	//if (NewIndex >= (signed)Nations.size())
	//	NewIndex = 0;
	//changeHumanPlayer(NewIndex);
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Nation::isHumanNation()
{
    return NationIndex == getNations().HumanNationIndex;
}

////////////////////////////////////////////////////////////
