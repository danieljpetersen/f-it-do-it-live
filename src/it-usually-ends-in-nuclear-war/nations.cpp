#include "nations.h"
#include "map.h"
#include "color_schemes.h"
#include "vision.h"

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Plugin_Init_Nations::work(const int Event)
{
    getNations().init();
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Nations::init()
{
	Nations.clear();
	getMap().CurrentMapLayout.StartingNumberOfNations = getMap().CurrentMapLayout.SuggestedNumberOfNations;
	const int NumberOfNations = getMap().CurrentMapLayout.StartingNumberOfNations;
	for (int i = 0; i < NumberOfNations; i++)
	{
		IUEINW::IUEINW_Nation Nation;

		Nation.NationIndex = i;
		Nation.Name = "";
		Nation.Color = getColorSchemes().getNationColor(i);
		Nation.IsAlive = true;

		Nation.EdgeTilesDiscovered.clear();
		Nation.EdgeTilesDiscovered.resize(4);
		Nation.EdgeTilesDiscovered[0].clear();
		Nation.EdgeTilesDiscovered[1].clear();
		Nation.EdgeTilesDiscovered[2].clear();
		Nation.EdgeTilesDiscovered[3].clear();
		Nation.TotalNumberOfUnitsInitialized = 0;
		//Nation.Units.clear();
		//Nation.Cities.clear();

		Nations.push_back(Nation);
	}

	getMap().CurrentMapLayout.StartingNumberOfNations = NumberOfNations;

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
}

////////////////////////////////////////////////////////////

IUEINW::IUEINW_Nation &IUEINW::IUEINW_Nations::getRandomNation()
{
    return Nations[fi::getRandom().i(0, Nations.size()-1)];
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
