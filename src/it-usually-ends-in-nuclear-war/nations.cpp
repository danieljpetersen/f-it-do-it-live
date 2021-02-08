#include "nations.h"
#include "map.h"
#include "color_schemes.h"
#include "vision.h"
#include "cities.h"
#include "drawable_map.h"
#include "user_interface.h"
#include "simulation.h"

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
	getVision().init();
	getSimulation().resetInput();

    Nations.clear();
	for (int NationIndex = 0; NationIndex < NumberOfNations; NationIndex++)
	{
		IUEINW::IUEINW_Nation Nation;

		Nation.NationIndex = NationIndex;
		Nation.Name = "";
		Nation.Color = getColorSchemes().getNationColor(NationIndex);
		Nation.IsAlive = true;

		Nation.TotalNumberOfUnitsInitialized = 0;
		Nation.Cities.clear();

		Nations.push_back(Nation);

		int i = fi::getRandom().i(0, (int)SpawnTiles.size()-1);
		int SpawnTile = SpawnTiles[i];
        int CityIndex = getTiles()[SpawnTile].CityIndex;
        SpawnTiles[i] = SpawnTiles.back();
        SpawnTiles.pop_back();
		getCities().setCity(CityIndex, NationIndex);

		// ---- spawn units
		{
			auto jsonStartingUnits = fi::getConfig()["game"]["units"]["starting-units"];

			for (auto jsonStartingUnit : jsonStartingUnits)
			{
				int PrototypeIndex = getUnits().prototypeNameToIndex(jsonStartingUnit);
				if (PrototypeIndex != -1)
				{
					getSimulation().NextSimulationInput->UnitCreationDestructionRequests.unitCreationRequest(NationIndex, SpawnTile, PrototypeIndex, false);
				}
			}
		}
	}

	getUnits().processCreationDestructionRequests(&getSimulation().NextSimulationInput->UnitCreationDestructionRequests);

	// ----

	HumanNationIndex = 0;
	Human = &Nations[HumanNationIndex];

	// ---- set camera position
    {
        auto CitiesWriteAccess =  getCities().Cities.getWriteAccess();
        auto CitiesWritePtr = CitiesWriteAccess.Data;
        int TileIndex = CitiesWritePtr->at(Human->Cities[0]).TileIndex;
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
	if ((isValidNation(NationIndex) == true) && (Nations[NationIndex].IsAlive) && (Nations[NationIndex].Cities.empty() != true))
	{
		getUI().SelectionManager.deselectUnits();
		HumanNationIndex = NationIndex;
		Human = &Nations[HumanNationIndex];

		getVision().recalculateVision();

		sf::Vector2f Center = getGrid().CommonCellData[Nations[NationIndex].getCapitalCityTileIndex()].Center;
		fi::getCanvasWorld().setCenter(Center);
		fi::getPlugins().delayedExecute(EVENT_BUILD_MAP_DRAWABLES);

		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Nations::isValidNation(int NationIndex)
{
	if (NationIndex > 0)
	{
		if (NationIndex < Nations.size())
		{
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Nations::changeHumanPlayerToNextNation()
{
	int NewIndex = HumanNationIndex + 1;
	if (NewIndex >= (signed)Nations.size())
		NewIndex = 0;
	return changeHumanPlayer(NewIndex);
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Nation::isHumanNation()
{
    return NationIndex == getNations().HumanNationIndex;
}

////////////////////////////////////////////////////////////

int IUEINW::IUEINW_Nation::getCapitalCityTileIndex()
{
	int CityIndex = Cities[0];
	return getCities().Cities.ReadPtr->at(CityIndex).TileIndex;
}

////////////////////////////////////////////////////////////
