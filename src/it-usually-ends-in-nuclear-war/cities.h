#ifndef FI_CITIES_H
#define FI_CITIES_H

#include "../common/plugins.h"

namespace IUEINW
{
    class IUEINW_City
    {
    public:
        std::vector<int> AllCityTiles; // in order that the city will acquire them
        std::vector<int> OwnedCityTiles;
        int TileIndex = -1;
        int NationIndex = -1;
        int TickCreated = -1;

        void acquireTile();
        void removeTile();
    };

    ////////////////////////////////////////////////////////////

    class IUEINW_Cities
    {
    public:
        std::vector<IUEINW_City> Cities;
        IUEINW_City   operator [](int i) const {return Cities[i];}
        IUEINW_City & operator [](int i)       {return Cities[i];}

        void setCity(int CityIndex, int NationIndex);

        void init();
        std::vector<int> getSpawnLocations_ReturnsTileIndexes(const int DesiredNationCount, const int MinimumNationSpawnDistance);
        unsigned long size() { return Cities.size(); }

    private:
        void createUnsettledCity(int TileIndex, std::vector<int> *CityTiles);
    };

    IUEINW_Cities &getCities();

    ////////////////////////////////////////////////////////////

    class IUEINW_Plugin_Init_Cities : public fi::Plugin_Base
    {
    public:
        void work(const int Event) override;
    };

    ////////////////////////////////////////////////////////////

    class IUEINW_Plugin_Draw_Cities : public fi::Plugin_Base
    {
    public:
        IUEINW_Plugin_Draw_Cities()
        {
            Lines.setPrimitiveType(sf::Lines);
            Quads.setPrimitiveType(sf::Quads);
        }

        void work(const int Event) override;

    private:
        sf::VertexArray Lines, Quads;
    };
}

#endif