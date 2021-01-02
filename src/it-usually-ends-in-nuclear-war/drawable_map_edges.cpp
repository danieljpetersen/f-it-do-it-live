#include <unordered_set>
#include "drawable_map_edges.h"
#include "color_schemes.h"
#include "map.h"
#include "cheats.h"
#include "nations.h"

void IUEINW::IUEINW_Drawable_Map_Edges::init()
{
	FrontDirtHeight = 25;
	Quads.setPrimitiveType(sf::Quads);
	Quads.clear();
}

//==================================

void IUEINW::IUEINW_Drawable_Map_Edges::draw()
{
	fi::getCanvasWorld().draw(Quads);
}

//==================================

void IUEINW::IUEINW_Drawable_Map_Edges::buildDrawable()
{
    Quads.clear();

    sf::Color LineEdgeColor = getColorSchemes().getMapEdgeLineColor();
    sf::Color LeftDirtColor = getColorSchemes().getLeftFrontDirtColor();
    sf::Color RightDirtColor = getColorSchemes().getRightFrontDirtColor();

    const int NUMBER_OF_MAP_EDGES = 4;
    for (int i = 0; i < NUMBER_OF_MAP_EDGES; i++)
    {
        int aCorner;
        int bCorner;
        bool AddDirt = false;
        sf::Color DirtColor;

        if (i == BOTTOMLEFT_EDGE)
        {
            if (getTiles().Grid.HorizontalWrap)
            {
                continue;
            }

            AddDirt = true;
            DirtColor = LeftDirtColor;
            aCorner = LEFT;
            bCorner = FRONT;
        }
        else if (i == BOTTOMRIGHT_EDGE)
        {
            AddDirt = true;
            DirtColor = RightDirtColor;
            aCorner = FRONT;
            bCorner = RIGHT;
        }
        else if (i == TOPLEFT_EDGE)
        {
            aCorner = LEFT;
            bCorner = BACK;
        }
        else // i == TOPRIGHT_EDGE
        {
            if (getTiles().Grid.HorizontalWrap)
            {
                continue;
            }

            aCorner = BACK;
            bCorner = RIGHT;
        }

        int TileIndex;
        std::vector<int> *Tiles = nullptr;

        if (getCheats().isRevealAllTerrainEnabled())
        {
            Tiles = &getTiles().EdgeTiles[i];
        }
        else if (getNations().Human != nullptr)
        {
            Tiles = &getNations().Human->EdgeTilesDiscovered[i];
        }
        else
        {
            Tiles = &getTiles().EdgeTiles[i];
        }

        for (int j = 0; j < Tiles->size(); j++)
        {
            TileIndex = Tiles->at(j);
            sf::Vector2f aPoint = getTiles().Grid.CommonCellData[TileIndex].Corners[aCorner];
            sf::Vector2f bPoint = getTiles().Grid.CommonCellData[TileIndex].Corners[bCorner];

            if (AddDirt)
            {
                sf::Vector2f cPoint = bPoint;
                sf::Vector2f dPoint = aPoint;

                cPoint.y += FrontDirtHeight;
                dPoint.y += FrontDirtHeight;
                Quads.append(sf::Vertex(aPoint, DirtColor));
                Quads.append(sf::Vertex(bPoint, DirtColor));
                Quads.append(sf::Vertex(cPoint, DirtColor));
                Quads.append(sf::Vertex(dPoint, DirtColor));
            }

            fi::Draw::line(Quads, aPoint, bPoint, LineEdgeColor, 2);
        }
    }
}