#include "drawable_grid_lines.h"
#include "map.h"
#include "color_schemes.h"
#include "mouse_tile_tracker.h"
#include "vision.h"
#include "controls.h"

void IUEINW::IUEINW_Drawable_Grid_Lines_Around_Mouse::onProgramStart()
{
	VertexArray.setPrimitiveType(sf::Lines);

	auto array = fi::getConfig()["visuals"]["grid-lines-enabled"];

	GridLinesAroundMouseEnabled = false;
	for (auto it = array.begin(); it != array.end(); ++it)
	{
		std::string StyleName = (*it).get<std::string>();
		if (StyleName == "around-mouse")
		{
			GridLinesAroundMouseEnabled = true;
		}
	}
}

//==================================

void IUEINW::IUEINW_Drawable_Grid_Lines_Around_Mouse::onDraw()
{
	fi::getCanvasWorld().draw(VertexArray);
}

//==================================

void IUEINW::IUEINW_Drawable_Grid_Lines_Around_Mouse::onUpdate()
{
	VertexArray.clear();

	//if (App.StateCitySelected.Active != true)
	{
		sf::Color Color;
		sf::Color GridLineInVisionColor = getColorSchemes().getGridLinesAroundMouseVisionColor();
		sf::Color GridLineOutVisionColor = getColorSchemes().getGridLinesAroundMouseNonVisionColor();
		int MaxBlackAlpha = GridLineOutVisionColor.a, MaxWhiteAlpha = GridLineInVisionColor.a;
		int Max = -1, Distance = -1, Percent =- 1;

		if (getMouseTileTracker().TileTransparencies.size())
		{
			for (std::list<Tile_Transparency_Tracker>::iterator iterator = getMouseTileTracker().TileTransparencies.begin(),
					end = getMouseTileTracker().TileTransparencies.end(); iterator != end; ++iterator)
			{
				 if (!getVision().hasHumanNationExplored((*iterator).AssociatedTile))
				 {
				 	continue;
				 }
				 else if (getVision().hasVision_IgnoreCheatMode((*iterator).AssociatedTile))
				 {
				 	Max = MaxWhiteAlpha;
				 	Color = GridLineInVisionColor;
				 }
				 else
				 {
				 	Max = MaxBlackAlpha;
				 	Color = GridLineOutVisionColor;
				 }

				if (GridLinesAroundMouseEnabled)
				{
					Distance = getTiles().Grid.CustomCellData[(*iterator).AssociatedTile].DistanceFromMousedTile;
					if (Distance > 9)
						Distance = 9;
					Percent = Distance * 10;
					//Color.a = sf::Uint8(fi::Math::getPercent(fi::Math::max(0, 100 - Percent), (*iterator).CurrentTransparency));
					Color.a = sf::Uint8(fi::Math::getPercent(std::max(0, 90 - Percent), (*iterator).CurrentTransparency)); // december 30th 2017 this was the value
					//Color.a = std::max(0, (*iterator).CurrentTransparency - 40);
					if (Color.a > Max)
						Color.a = sf::Uint8(Max);

					if (Color.a > 0)
					{
						getTiles().Grid.buildVertexForCell_Lines((*iterator).AssociatedTile, Color, VertexArray);
					}
				}
			}
		}
	}
}

//==================================
//==================================
//==================================

void IUEINW::IUEINW_Drawable_Grid_Lines_Always_On::onProgramStart()
{
	VBO.setPrimitiveType(sf::Lines);
	VBO.setUsage(sf::VertexBuffer::Dynamic);
	Vertices.reserve(getTiles().size() * 8);
	VBO.create(Vertices.size());

	auto jsonContent = fi::getConfig()["visuals"];

	// ---- grid-lines-enabled
	{
		GridLinesAlwaysOnNonVision = false;
		GridLinesAlwaysOnInVision = false;

		auto array = jsonContent["grid-lines-enabled"];
		for (auto it = array.begin(); it != array.end(); ++it)
		{
			std::string StyleName = (*it).get<std::string>();
			if (StyleName == "always-on-nonvision")
			{
				GridLinesAlwaysOnNonVision = true;
			}
			else if (StyleName == "always-on-in-vision")
			{
				GridLinesAlwaysOnInVision = true;
			}
		}
	}
}

//==================================

void IUEINW::IUEINW_Drawable_Grid_Lines_Always_On::onDraw()
{
	//if (getUI().areWeShowingNoTextDueToZoomLevel())
	//{
	//	return;
	//}

	if (Vertices.size())
	{
		fi::getCanvasWorld().draw(VBO);
	}
}

//==================================

void IUEINW::IUEINW_Drawable_Grid_Lines_Always_On::onUpdate()
{
	sf::Color Color;
	sf::Color InVisionColor = getColorSchemes().getGridLinesAlwaysOnVisionColor();
	sf::Color OutVisionColor = getColorSchemes().getGridLinesAlwaysOnNonVisionColor();

	Vertices.clear();
	VBO.create(0);

    if ((GridLinesAlwaysOnInVision) || (GridLinesAlwaysOnNonVision))
    {
        std::vector<int> *TilesWithGridLines = nullptr;
        if (getColorSchemes().getGridLinesAlwaysOnTileGrouping() == Tile_Type_Grouping::ALL_TILE_TYPES)
        {
            TilesWithGridLines = &getTiles().AllTiles;
        }
        else if (getColorSchemes().getGridLinesAlwaysOnTileGrouping() == Tile_Type_Grouping::OCEAN_TILE_TYPES)
        {
            TilesWithGridLines = &getTiles().WaterTiles;
        }
        else if (getColorSchemes().getGridLinesAlwaysOnTileGrouping() == Tile_Type_Grouping::LAND_TILE_TYPES)
        {
            TilesWithGridLines = &getTiles().LandTiles;
        }
        else
        {
            TilesWithGridLines = &getTiles().AllTiles;
        }


        int _Count = 0;
        for (unsigned int i = 0; i < TilesWithGridLines->size(); i++)
        {
            int TileIndex = TilesWithGridLines->at(i);
            {
                 if (getVision().hasHumanNationExplored(TileIndex))
                {
                     if ((getVision().hasVision_IgnoreCheatMode(TileIndex) && (GridLinesAlwaysOnInVision)))
                     {
                        Color = InVisionColor;
                     }
                     else if (GridLinesAlwaysOnNonVision)
                     {
                        Color = OutVisionColor;
                     }
                     else
                     {
                        continue;
                     }

                    int VBOIndex = _Count++ * 8;

                    Vertices.push_back(sf::Vertex(getTiles().Grid.CommonCellData[TileIndex].Corners[LEFT], Color));
                    Vertices.push_back(sf::Vertex(getTiles().Grid.CommonCellData[TileIndex].Corners[BACK], Color));

                    Vertices.push_back(Vertices[VBOIndex+1]);
                    Vertices.push_back(sf::Vertex(getTiles().Grid.CommonCellData[TileIndex].Corners[RIGHT], Color));

                    Vertices.push_back(Vertices[VBOIndex+3]);
                    Vertices.push_back(sf::Vertex(getTiles().Grid.CommonCellData[TileIndex].Corners[FRONT], Color));

                    Vertices.push_back(Vertices[VBOIndex+5]);
                    Vertices.push_back(Vertices[VBOIndex]);
                }
            }
        }

        VBO.update(&Vertices[0], Vertices.size(), 0);
    }
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Drawable_Map_Land_Ocean_Border::addLandOceanBorderIfApplicable(int i)
{
	// todo, you know this shit can all be cached in terms of checking neighbors for border.
		//  it's not because of vision concerns, i think, but that doesn't really matter
	sf::Color LandOceanBorderLineColor  = getColorSchemes().getLandOceanBorderLineColor();
	if (LandOceanBorderLineColor.a != 0)
	{
		if (getTiles().Grid.CustomCellData[i].TileType == COASTAL_OCEAN)
		{
			// Using getTileNeighbor rather than getCellIndex due to horizontal wrap worries
			int LeftNeighbor = getTiles().Grid.getTileNeighbor(i, TileToLeft);
			int RightNeighbor = getTiles().Grid.getTileNeighbor(i, TileToRight);
			int TopNeighbor = getTiles().Grid.getTileNeighbor(i, TileToTop);
			int BottomNeighbor = getTiles().Grid.getTileNeighbor(i, TileToBottom);

			if (getTiles().Grid.isValidCell(LeftNeighbor))
			{
				if (getTiles().Grid.CustomCellData[LeftNeighbor].isWaterTile() != true)
				{
					sf::Vertex vert;
					vert.color = LandOceanBorderLineColor;
					vert.position = getTiles().Grid.CommonCellData[LeftNeighbor].getBack2f();
					Lines.append(vert);
					vert.position = getTiles().Grid.CommonCellData[LeftNeighbor].getRight2f();
					Lines.append(vert);

				}
			}

			if (getTiles().Grid.isValidCell(RightNeighbor))
			{
				if (getTiles().Grid.CustomCellData[RightNeighbor].isWaterTile() != true)
				{
					sf::Vertex vert;
					vert.color = LandOceanBorderLineColor ;
					vert.position = getTiles().Grid.CommonCellData[RightNeighbor].getLeft2f();
					Lines.append(vert);
					vert.position = getTiles().Grid.CommonCellData[RightNeighbor].getFront2f();
					Lines.append(vert);
				}
			}

			if (getTiles().Grid.isValidCell(TopNeighbor))
			{
				if (getTiles().Grid.CustomCellData[TopNeighbor].isWaterTile() != true)
				{
					sf::Vertex vert;
					vert.color = LandOceanBorderLineColor ;
					vert.position = getTiles().Grid.CommonCellData[TopNeighbor].getFront2f();
					Lines.append(vert);
					vert.position = getTiles().Grid.CommonCellData[TopNeighbor].getRight2f();
					Lines.append(vert);
				}
			}

			if (getTiles().Grid.isValidCell(BottomNeighbor))
			{
				if (getTiles().Grid.CustomCellData[BottomNeighbor].isWaterTile() != true)
				{
					sf::Vertex vert;
					vert.color = LandOceanBorderLineColor ;
					vert.position = getTiles().Grid.CommonCellData[BottomNeighbor].getLeft2f();
					Lines.append(vert);
					vert.position = getTiles().Grid.CommonCellData[BottomNeighbor].getBack2f();
					Lines.append(vert);
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Drawable_Map_Land_Ocean_Border::onUpdate()
{
	Lines.clear();
	for (int TileIndex = 0; TileIndex < getTiles().size(); TileIndex++)
	{
		if (getVision().hasHumanNationExplored(TileIndex))
		{
			addLandOceanBorderIfApplicable(TileIndex);
		}
	}
}
