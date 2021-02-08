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
        bool IsAlive;
        std::string Name;
        int NationIndex;
        int TotalNumberOfUnitsInitialized;
        sf::Color Color;
	    std::vector<int> Cities; // functions as index into getCities().Cities; ie:  getCities.Cities[Nation->Cities[0]];
		bool isHumanNation();
		int getCapitalCityTileIndex();
	};

	////////////////////////////////////////////////////////////

	class IUEINW_Nations
	{
	public:
		IUEINW_Nation   operator [](int i) const {return Nations[i];}
		IUEINW_Nation & operator [](int i)       {return Nations[i];}

		std::vector<IUEINW_Nation> Nations;
		int HumanNationIndex = 0;
		IUEINW_Nation *Human = nullptr;

		void init();
		unsigned int size() { return (unsigned int)Nations.size(); }
		bool isValidNation(int NationIndex);
		bool changeHumanPlayer(int NationIndex);
		bool changeHumanPlayerToNextNation();
		IUEINW_Nation &getRandomNation();
	};

    ////////////////////////////////////////////////////////////

    IUEINW_Nations &getNations();
    int getHumanNationIndex();
}
#endif