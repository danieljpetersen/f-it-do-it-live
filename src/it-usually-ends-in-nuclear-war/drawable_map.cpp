#include <unordered_set>
#include "drawable_map.h"
#include "map.h"
#include "color_schemes.h"
#include "vision.h"

void IUEINW::IUEINW_Plugin_Init_Map_Drawables::work(const int Event)
{
    getMapDrawables().DrawableGridLinesAlwaysOn.init();
    getMapDrawables().DrawableGridLinesAroundMouse.init();
    getMapDrawables().DrawableMapEdges.init();
    getMapDrawables().DrawableMapLandOceanBorder.init();
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Plugin_Build_Map_Drawables::work(const int Event)
{
    getMapDrawables().buildMapDrawable();
    getMapDrawables().DrawableGridLinesAlwaysOn.buildDrawable();
    getMapDrawables().DrawableMapEdges.buildDrawable();
    getMapDrawables().DrawableMapLandOceanBorder.buildDrawable();
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Plugin_Draw_Map::work(const int Event)
{
    getMapDrawables().DrawableGridLinesAroundMouse.buildDrawable(); // updated per each frame

    getMapDrawables().draw();
    getMapDrawables().DrawableGridLinesAlwaysOn.draw();
    getMapDrawables().DrawableGridLinesAroundMouse.draw();
    getMapDrawables().DrawableMapEdges.draw();
    getMapDrawables().DrawableMapLandOceanBorder.draw();
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Map_Drawables::buildMapDrawable()
{
    VBO.clear();
    Vertices.clear();

    fi::Grid_1D_Custom_Data<IUEINW_Tile> *Grid = &getMap().Tiles.Grid;

    VBO.resize(Grid->NumberOfColumns);
    Vertices.resize(Grid->NumberOfColumns);

    for (int ColIndex = 0; ColIndex < Grid->NumberOfColumns; ColIndex++)
    {
        Vertices[ColIndex].resize(Grid->NumberOfRows * 6);
        VBO[ColIndex].create(Vertices[ColIndex].size());
        VBO[ColIndex].setPrimitiveType(sf::Triangles);
        VBO[ColIndex].setUsage(sf::VertexBuffer::Stream);

        for (int RowIndex = 0; RowIndex < Grid->NumberOfRows; RowIndex++)
        {
            int TileIndex = getTiles().Grid.getCellIndex(ColIndex, RowIndex);
            sf::Vertex LeftPoint = sf::Vertex(Grid->CommonCellData[TileIndex].getLeft2f(), getColorSchemes().getUnexplored());
            sf::Vertex TopPoint = sf::Vertex(Grid->CommonCellData[TileIndex].getBack2f(), getColorSchemes().getUnexplored());
            sf::Vertex RightPoint = sf::Vertex(Grid->CommonCellData[TileIndex].getRight2f(), getColorSchemes().getUnexplored());
            sf::Vertex BottomPoint = sf::Vertex(Grid->CommonCellData[TileIndex].getFront2f(), getColorSchemes().getUnexplored());

            int VBOIndex = RowIndex * 6;
            if ((Grid->CustomCellData[TileIndex].isFlat()))
            {
                Vertices[ColIndex][VBOIndex] = RightPoint;
                Vertices[ColIndex][VBOIndex + 1] = TopPoint;
                Vertices[ColIndex][VBOIndex + 2] = LeftPoint;

                Vertices[ColIndex][VBOIndex + 3] = RightPoint;
                Vertices[ColIndex][VBOIndex + 4] = BottomPoint;
                Vertices[ColIndex][VBOIndex + 5] = LeftPoint;
            }
            else
            {
                //one shade for top, one shade for bottom
                if (Grid->CustomCellData[TileIndex].HorizontalSplit)
                {
                    Vertices[ColIndex][VBOIndex] = LeftPoint;
                    Vertices[ColIndex][VBOIndex + 1] = TopPoint;
                    Vertices[ColIndex][VBOIndex + 2] = RightPoint;

                    Vertices[ColIndex][VBOIndex + 3] = LeftPoint;
                    Vertices[ColIndex][VBOIndex + 4] = RightPoint;
                    Vertices[ColIndex][VBOIndex + 5] = BottomPoint;
                }

                    //one shade for left, one shade for right
                else if (Grid->CustomCellData[TileIndex].VerticalSplit)
                {
                    Vertices[ColIndex][VBOIndex] = TopPoint;
                    Vertices[ColIndex][VBOIndex + 1] = LeftPoint;
                    Vertices[ColIndex][VBOIndex + 2] = BottomPoint;

                    Vertices[ColIndex][VBOIndex + 3] = TopPoint;
                    Vertices[ColIndex][VBOIndex + 4] = RightPoint;
                    Vertices[ColIndex][VBOIndex + 5] = BottomPoint;
                }
            }

            setTileVertexColors(TileIndex);
        }

        VBO[ColIndex].update(&Vertices[ColIndex][0], Vertices[ColIndex].size(), 0);
    }
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Map_Drawables::draw()
{
    for (int ColIndex = 0; ColIndex < VBO.size(); ColIndex++)
    {
        VBO[ColIndex].update(&Vertices[ColIndex][0], Vertices[ColIndex].size(), 0);
    }

    for (int ColIndex = 0; ColIndex < VBO.size(); ColIndex++)
    {
        fi::getCanvasWorld().getRenderTarget()->draw(VBO[ColIndex], getTiles().Grid.ColumnTransforms[ColIndex]);
    }
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Map_Drawables::setTileVertexColors()
{
    const int NumTiles = getTiles().Grid.NumberOfCells;
    for (int i = 0; i < NumTiles; i++)
    {
        setTileVertexColors(i);
    }
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Map_Drawables::setTileVertexColors(int TileIndex)
{
    if (getVision().hasHumanNationExplored(TileIndex))
    {
        setTileVertexColors(TileIndex, true);
    }
    else
    {
        setTileVertexColors(TileIndex, false);
    }
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Map_Drawables::setTileVertexColors(int TileIndex, bool ExploredTile)
{
    fi::Grid_1D_Custom_Data<IUEINW_Tile> *Grid = &getMap().Tiles.Grid;

    int ColIndex = getTiles().Grid.getCol(TileIndex);
    int RowIndex = getTiles().Grid.getRow(TileIndex);
    int VBOIndex = RowIndex * 6;

    if (ExploredTile)
    {
        sf::Color aColor = Grid->CustomCellData[TileIndex].aColor;
        sf::Color bColor = Grid->CustomCellData[TileIndex].bColor;

        aColor = Grid->CustomCellData[TileIndex].DebugColor;
        bColor = Grid->CustomCellData[TileIndex].DebugColor;

        //aColor = fi::getRandom().color(); // test for when drawable updates

        if ((Grid->CustomCellData[TileIndex].isFlat()))
        {
            Vertices[ColIndex][VBOIndex].color = aColor;
            Vertices[ColIndex][VBOIndex + 1].color = aColor;
            Vertices[ColIndex][VBOIndex + 2].color = aColor;

            Vertices[ColIndex][VBOIndex + 3].color = aColor;
            Vertices[ColIndex][VBOIndex + 4].color = aColor;
            Vertices[ColIndex][VBOIndex + 5].color = aColor;
        }
        else
        {
            //one shade for top, one shade for bottom
            if (Grid->CustomCellData[TileIndex].HorizontalSplit)
            {
                Vertices[ColIndex][VBOIndex].color = aColor;
                Vertices[ColIndex][VBOIndex + 1].color = aColor;
                Vertices[ColIndex][VBOIndex + 2].color = aColor;

                //======

                Vertices[ColIndex][VBOIndex + 3].color = bColor;
                Vertices[ColIndex][VBOIndex + 4].color = bColor;
                Vertices[ColIndex][VBOIndex + 5].color = bColor;
            }

            //one shade for left, one shade for right
            if (Grid->CustomCellData[TileIndex].VerticalSplit)
            {
                Vertices[ColIndex][VBOIndex].color = aColor;
                Vertices[ColIndex][VBOIndex + 1].color = aColor;
                Vertices[ColIndex][VBOIndex + 2].color = aColor;

                //======
                Vertices[ColIndex][VBOIndex + 3].color = bColor;
                Vertices[ColIndex][VBOIndex + 4].color = bColor;
                Vertices[ColIndex][VBOIndex + 5].color = bColor;
            }
        }
    }
    else
    {
        sf::Color UnexploredMapColor = getColorSchemes().getUnexplored();

        Vertices[ColIndex][VBOIndex].color = UnexploredMapColor;
        Vertices[ColIndex][VBOIndex + 1].color = UnexploredMapColor;
        Vertices[ColIndex][VBOIndex + 2].color = UnexploredMapColor;

        Vertices[ColIndex][VBOIndex + 3].color = UnexploredMapColor;
        Vertices[ColIndex][VBOIndex + 4].color = UnexploredMapColor;
        Vertices[ColIndex][VBOIndex + 5].color = UnexploredMapColor;
    }
}
