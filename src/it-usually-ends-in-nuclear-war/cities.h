#ifndef FI_CITIES_H
#define FI_CITIES_H

#include "../common/plugins.h"
#include "../common/double_buffer.h"

namespace IUEINW
{
    class IUEINW_City
    {
    public:
        int CityIndex = -1;
        int TileIndex = -1;
        int NationIndex = -1;
        int TickCreated = -1;

        int OwnedTileIndex = 0; // corresponds to getCities().OwnedTiles[CityIndex] where elements < OwnedTileIndex are owned by the city.

        void acquireTile();
        void removeTile();
    };

    ////////////////////////////////////////////////////////////

    class IUEINW_Cities
    {
    public:
        void init();

        fi::Double_Buffer<std::vector<IUEINW_City>> Cities;

        std::vector<std::vector<int>> CityTiles; // stored separately because it's unchanging data after map generation; no need to copy each tick

        void setCity(int CityIndex, int NationIndex);

        std::vector<int> getSpawnLocations_ReturnsTileIndexes(const int DesiredNationCount, const int MinimumNationSpawnDistance);

    private:
        void createUnsettledCity(int TileIndex, std::vector<int> *CityTiles);
    };

    IUEINW_Cities &getCities();

    ////////////////////////////////////////////////////////////

    class IUEINW_Plugin_Init_Cities : public fi::Plugin_Base
    {
    public:
        void work(int Event) override;
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