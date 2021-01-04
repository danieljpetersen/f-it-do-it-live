#ifndef IUEINW_NATIONS_H
#define IUEINW_NATIONS_H

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "globals.h"

namespace IUEINW
{
    ////////////////////////////////////////////////////////////

    class IUEINW_Plugin_Init_Nations : public fi::Plugin_Base
    {
    public:
        void work(const int Event) override;
    };

    ////////////////////////////////////////////////////////////

    class IUEINW_Nation
	{
	public:
		std::vector<std::vector<int>> EdgeTilesDiscovered; // todo should be in vision
		bool IsAlive;
		sf::Color Color;
		std::string Name;
		int NationIndex;
		int TotalNumberOfUnitsInitialized;

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

		void getUnitAttritionImmunities(bool &ImmuneToIce, bool &ImmuneToWater, bool &ImmuneToDesert) // todo
        {
			ImmuneToDesert = false;
			ImmuneToWater = false;
			ImmuneToIce = false;
		}
	};

	////////////////////////////////////////////////////////////

	class IUEINW_Nations
	{
	public:
		IUEINW_Nation   operator [](int i) const {return Nations[i];}
		IUEINW_Nation & operator [](int i)       {return Nations[i];}

		std::vector<IUEINW_Nation> Nations;
		int HumanNationIndex = -1;
		IUEINW_Nation *Human = nullptr;

		void init();
		unsigned int size() { return (unsigned int)Nations.size(); }
		bool changeHumanPlayer(int NationIndex);
		void changeHumanPlayerToNextNation();
		IUEINW_Nation &getRandomNation();
	};

    ////////////////////////////////////////////////////////////

    IUEINW_Nations &getNations();
}
#endif