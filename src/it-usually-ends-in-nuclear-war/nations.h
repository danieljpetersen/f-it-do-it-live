#ifndef IUEINW_NATIONS_H
#define IUEINW_NATIONS_H

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "globals.h"

namespace IUEINW
{
	class IUEINW_Nation
	{
	public:
		std::vector<std::vector<int>> EdgeTilesDiscovered;

		bool IsAlive;
		sf::Color Color;
		std::string Name;
		int NationIndex;
		int NumberOfUnitsInitialized;

		bool isHumanNation();

		bool discoveredBottomLeftEdge()
		{
			return (bool)EdgeTilesDiscovered[BOTTOMLEFT_EDGE].empty();
		}

		bool discoveredBottomRightEdge()
		{
			return (bool)EdgeTilesDiscovered[BOTTOMRIGHT_EDGE].empty();
		}

		bool discoveredTopLeftEdge()
		{
			return (bool)EdgeTilesDiscovered[TOPLEFT_EDGE].empty();
		}

		bool discoveredTopRightEdge()
		{
			return (bool)EdgeTilesDiscovered[TOPRIGHT_EDGE].empty();
		}

		void getUnitAttritionImmunities(bool &ImmuneToIce, bool &ImmuneToWater, bool &ImmuneToDesert)
		{
			// todo
			ImmuneToDesert = false;
			ImmuneToWater = false;
			ImmuneToIce = false;
		}
		//
		std::vector<int> getCollectionOfCityTiles()
		{
			std::vector<int> CityTiles;
			//for (int i = 0; i < Cities.size(); i++)
			{
				// CityTiles.push_back(App.Cities.get(Cities[i])->TileIndex); // todo
			}
			return CityTiles;
		}
	};

	////////////////////////////////////////////////////////////

	class IUEINW_Nations : public fi::Plugin_Base
	{
	public:
		IUEINW_Nation   operator [](int i) const {return Nations[i];}
		IUEINW_Nation & operator [](int i)       {return Nations[i];}

		std::vector<IUEINW_Nation> Nations;

		int HumanNationIndex = -1;
		IUEINW_Nation *Human = nullptr;

		unsigned int size() { return Nations.size(); }

		bool changeHumanPlayer(int NationIndex);

		void changeHumanPlayerToNextNation();
	private:
		void onMapGeneration();

		friend class IUEINW_Map_Generator;
	};

	IUEINW_Nations &getNations();
}
#endif