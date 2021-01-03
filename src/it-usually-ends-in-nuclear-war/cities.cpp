#include "cities.h"
#include "vision.h"
#include "map.h"
#include "nations.h"
#include "color_schemes.h"

void IUEINW::IUEINW_Plugin_Init_Cities::work(const int Event)
{
    getCities().init();
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Plugin_Draw_Cities::work(const int Event)
{
    Lines.clear();
    Quads.clear();

    int TileIndex;
    sf::Color NationColor;
    for (int i = 0; i < getCities().Cities.size(); i++)
    {
        TileIndex = getCities()[i].TileIndex;

        if (getVision().canHumanSeeCity(TileIndex, getCities()[i].TickCreated))
        {
            getTiles().Grid.buildVertexForCell_Quad(TileIndex, getColorSchemes().getCityUndersideColor(), Quads); // underneath city

            if (getColorSchemes().doesExplicitCityColorExist())
            {
                getTiles().Grid.buildVertexForCell_QuadOffset(TileIndex, getColorSchemes().getExplicitCityColor(), getColorSchemes().getCityQuadOffset(), Quads);
            }
            else
            {
                getTiles().Grid.buildVertexForCell_QuadOffset(TileIndex, getNations()[getCities()[i].NationIndex].Color, getColorSchemes().getCityQuadOffset(), Quads);
            }
        }
    }

    fi::getCanvasWorld().draw(Quads);
    fi::getCanvasWorld().draw(Lines);
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Cities::init()
{
    // around 15k land tiles per a normal sized map
    // we want to spawn around 100 cities.
    // spawn cities and assign surrounding land as belonging to relevant city

    Cities.clear();

    std::map<int, int> CityIndexMappedToNodeIndex; // key: CityIndex; value: NodeIndex....
    int CurrentCityIndex = 0;
    for (int ContinentIndex = 0; ContinentIndex < getMap().Tiles.Continents.size(); ContinentIndex++)
    {
        std::vector<int> SpreadNodes;
        std::vector<sf::Color> DebugColors;
        std::vector<int> TileCountPerNode;

        // ---- populate this continents initial batch of SpreadNodes
        {
            const int DesiredInitialSpreadNodeCount = std::max(1, (int)(getMap().Tiles.Continents[ContinentIndex].size() / 10));
            int TryCount = 0;
            for (int i = 0; i < DesiredInitialSpreadNodeCount; i++)
            {
                int PotentialTile = getMap().Tiles.getRandomLandTile(ContinentIndex);

                bool Valid = true;
                if (fi::vectorContains(&SpreadNodes, PotentialTile))
                {
                    Valid = false;
                }
                else if (SpreadNodes.empty() != true)
                {
                    auto ClosestTile = SpreadNodes[getMap().Tiles.findClosestTileFromTileIndex_returnsLocalIndex(SpreadNodes, PotentialTile, false)];
                    if (getGrid().getDistanceBetweenTiles(ClosestTile, PotentialTile) < 14)
                    {
                        Valid = false;
                    }
                }

                if (!Valid)
                {
                    i--;
                }
                else
                {
                    SpreadNodes.push_back(PotentialTile);
                    DebugColors.push_back(fi::getRandom().color());

                    getMap().Tiles[PotentialTile].DebugColor = DebugColors.back();
                    getMap().Tiles[PotentialTile].CityIndex = CurrentCityIndex++;
                    CityIndexMappedToNodeIndex[getMap().Tiles[PotentialTile].CityIndex] = (int)SpreadNodes.size()-1;
                    TileCountPerNode.push_back(1);
                    //getCities().addCity(PotentialTile);
                }

                TryCount++;
                if (TryCount > DesiredInitialSpreadNodeCount * 3)
                {
                    break;
                }
            }
        }

        // ---- spread the nodes - this was the easiest thing i could think of, not performant but doesn't matter as this only happens map gen
        {
            std::vector<std::vector<int>> DistancesToSpreadNodes(SpreadNodes.size(), std::vector<int>());
            for (int i = 0; i < SpreadNodes.size(); i++)
            {
                for (int j = 0; j < getTiles().Continents[ContinentIndex].size(); j++)
                {
                    int TileIndex = getTiles().Continents[ContinentIndex][j];
                    DistancesToSpreadNodes[i].push_back(getGrid().getDistanceBetweenTiles(SpreadNodes[i], TileIndex));
                }
            }

            int TotalIterationCount = 0;
            while (true)
            {
                TotalIterationCount++;
                bool Assigned = false;
                for (int i = 0; i < SpreadNodes.size(); i++)
                {
                    int BestIndex = -1;
                    int BestDistance = -1;

                    for (int j = 0; j < DistancesToSpreadNodes[i].size(); j++)
                    {
                        //if (DoneNodes[i] != true)
                        if ((DistancesToSpreadNodes[i][j] < BestDistance) || (BestDistance == -1))
                        {
                            int TileIndex = getTiles().Continents[ContinentIndex][j];
                            if (getTiles()[TileIndex].CityIndex == -1)
                            {
                                int NeighborInGroupCount = 0;
                                auto Neighbors = &getGrid().Neighbors[TileIndex];
                                for (auto Neighbor : *Neighbors)
                                {
                                    if (getTiles()[Neighbor].DebugColor == DebugColors[i])
                                    {
                                        NeighborInGroupCount++;
                                    }
                                }

                                if (NeighborInGroupCount > 0) // to prevent jumping across water to tiles on same continent
                                {
                                    if ((NeighborInGroupCount >= 2) || (TotalIterationCount < 20))
                                    {
                                        BestIndex = j;
                                        BestDistance = DistancesToSpreadNodes[i][j];
                                    }
                                }
                            }
                        }
                    }

                    if (BestIndex != -1)
                    {
                        Assigned = true;
                        int TileIndex = getTiles().Continents[ContinentIndex][BestIndex];
                        getTiles()[TileIndex].DebugColor = DebugColors[i];
                        getTiles()[TileIndex].CityIndex = getTiles()[SpreadNodes[i]].CityIndex;
                        TileCountPerNode[i]++;
                    }
                }

                if (!Assigned)
                {
                    break;
                }
            }
        }

        // ---- land at the fringes may still be empty at this point. ensure everything is owned
        {
            bool UnassignedTiles = true;
            while (UnassignedTiles)
            {
                UnassignedTiles = false;
                for (int i = 0; i < getMap().Tiles.Continents[ContinentIndex].size(); i++)
                {
                    int TileIndex = getMap().Tiles.Continents[ContinentIndex][i];
                    if (getTiles()[TileIndex].CityIndex == -1)
                    {
                        auto Neighbors = getGrid().Neighbors[TileIndex];
                        for (auto Neighbor : Neighbors)
                        {
                            // for now just give it to the first owned neighbor we find.
                            // ideally it would assign a tile to the least populated city

                            if (getTiles()[Neighbor].CityIndex != -1)
                            {
                                int NodeIndex = CityIndexMappedToNodeIndex[getTiles()[Neighbor].CityIndex];
                                getTiles()[TileIndex].DebugColor = DebugColors[NodeIndex];
                                getTiles()[TileIndex].CityIndex = getTiles()[Neighbor].CityIndex;
                                TileCountPerNode[NodeIndex]++;
                                break;
                            }
                        }

                        if (getTiles()[TileIndex].CityIndex == -1)
                        {
                            UnassignedTiles = true;
                        }
                    }
                }
            }
        }

        // ---- create cities
        {
            // determine city position; averaged row / tile per tile grouping to average to get centerish tile
            std::vector<std::pair<int, int>> RowColumnWeights(SpreadNodes.size(), std::pair<int, int>(0, 0));
            for (int i = 0; i < getMap().Tiles.Continents[ContinentIndex].size(); i++)
            {
                int TileIndex = getTiles().Continents[ContinentIndex][i];
                int CityIndex = getTiles()[TileIndex].CityIndex;
                int NodeIndex = CityIndexMappedToNodeIndex[CityIndex];

                RowColumnWeights[NodeIndex].first += getGrid().CommonCellData[TileIndex].Row;
                RowColumnWeights[NodeIndex].second += getGrid().CommonCellData[TileIndex].Column;
            }

            for (int i = 0; i < RowColumnWeights.size(); i++)
            {
                int Row = RowColumnWeights[i].first / TileCountPerNode[i];
                int Col = RowColumnWeights[i].second / TileCountPerNode[i];

                int TileIndex = getGrid().getCellIndex(Col, Row);

                // averaged center tile fell in the ocean or something
                if (getTiles()[TileIndex].CityIndex != SpreadNodes[i])
                {
                    // just find the closest tile
                    int ClosestTileIndex = -1;
                    int BestDistance = -1;

                    for (int j = 0; j < getMap().Tiles.Continents[ContinentIndex].size(); j++)
                    {
                        int OtherTileIndex = getMap().Tiles.Continents[ContinentIndex][j];
                        if (getTiles()[SpreadNodes[i]].CityIndex == getTiles()[OtherTileIndex].CityIndex)
                        {
                            auto Distance = getGrid().getDistanceBetweenTiles(TileIndex, OtherTileIndex);
                            if ((Distance < BestDistance) || (ClosestTileIndex == -1))
                            {
                                BestDistance = Distance;
                                ClosestTileIndex = OtherTileIndex;
                            }
                        }
                    }

                    TileIndex = ClosestTileIndex;
                }

                addCity(TileIndex);
            }
        }
    }
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Cities::addCity(int TileIndex)
{
    IUEINW_City City;
    City.NationIndex = 0;
    City.TickCreated = 0;
    City.TileIndex = TileIndex;
    Cities.push_back(City);
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_City::acquireTile()
{

}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_City::removeTile()
{

}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_City::setOwner(int NationIndex)
{
    NationIndex = NationIndex;
}