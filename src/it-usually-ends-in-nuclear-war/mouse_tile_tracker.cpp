#include "mouse_tile_tracker.h"
#include "../common/engine.h"
#include "map.h"
#include "color_schemes.h"

void IUEINW::IUEINW_Mouse_Tile_Tracker::onUpdate()
{
	findMouseTileIndex();
    updateMouseHighlightingTiles();
    updateTileTransparenciesList();
}

//==================================

void IUEINW::IUEINW_Mouse_Tile_Tracker::updateMouseHighlightingTiles()
{
	MouseHighlightingTiles.clear();

    // todo -- does not work
	//if (fi::StateManager.isActive(&App.CityScreen))
	//{
     //   IUEINW_City *SelectedCity = App.Cities.get(App.Cities.getSelectedCity());
		//for (int i = 0; i < SelectedCity->TilesOwned.size(); i++)
		//{
		//	int Index = SelectedCity->TilesOwned[i];
		//	MouseHighlightingTiles.push_back(Index);
		//}
	//}
	//else
	{
		int OriginalIndex = MouseTileIndex;

		sf::Color Color = sf::Color::Black;
		sf::Color Black = sf::Color::Black;
		sf::Color White = sf::Color(180, 180, 180, 15);

		sf::Time CurrentTimeClock = fi::getEngine().Clock.getElapsedTime();

		if (getTiles().Grid.isValidTile(OriginalIndex))
		{
			// todo -- THIS WAS COMMENTED AT START OF UNCOMMENT
			// Highlight entire province one color if the mouse is inside a city name
//		if (App.Cities.HighlightingProvinceNameTag.mouseIsInsideProvinceName())
//		{
//			sub::Vector_Int_Wrapper CoT = App.Cities.Objects[App.Cities.HighlightingProvinceNameTag.CityIndex].OwnedTilesInProvince;
//			for (unsigned int i = 0; i < CoT.size(); i++)
//			{
//				int Index = CoT.getValueByLocalIndex(i);
//				if (getTiles().Tiles[Index].getTileType() != OCEAN)
//				{
//					getTiles().Tiles[Index].TimeOfBeingInLastMousedArea = CurrentTimeClock;
//					getTiles().Tiles[Index].DistanceFromMousedTile = getTiles().getDistanceBetweenTiles(OriginalIndex, Index);
//					getTiles().MouseHighlightingTiles.push_back(Index);
//				}
//			}
//		}
//		else
			{
				int CoTSize;
				int AreaSize = 8;
				//int AreaSize = 7;
				std::vector<int> *CoT = getTiles().getArea(OriginalIndex, AreaSize, getColorSchemes().getGridLinesAroundMouseTileGrouping(), CoTSize);
				for (unsigned int i = 0; i < CoTSize; i++)
				{
					int Index = CoT->at(i);
					//if (getTiles().Tiles[Index].isWaterTile() != true)
					{
						getTiles().Grid.CustomCellData[Index].DistanceFromMousedTile = getTiles().Grid.getDistanceBetweenTiles(OriginalIndex, Index);
						MouseHighlightingTiles.push_back(Index);
					}
				}
			}
		}
	}
}

//==================================

void IUEINW::IUEINW_Mouse_Tile_Tracker::updateTileTransparenciesList()
{
	sf::Time CurrentTimeClock = fi::getEngine().Clock.getElapsedTime();
	sf::Int32 TheCurrentTime = CurrentTimeClock.asMilliseconds();

	// Step 1)  Decrement transparency of all
	// Step 2)  Remove any drawables with transparency of 0
	if (TileTransparencies.size() > 0)
	{
		std::list<Tile_Transparency_Tracker>::iterator iterator = TileTransparencies.begin();
		while (iterator != TileTransparencies.end())
		{
			sf::Int32 Diff = (TheCurrentTime - (*iterator).TimeAdded.asMilliseconds()) / 5;
			sf::Int32 Value = (255 - Diff);
			if (Value <= 0)
				Value = 0;

			(*iterator).CurrentTransparency = Value;

			// remove if applicable
			if ((*iterator).CurrentTransparency <= 0)
			{
				TileTransparencies.erase(iterator++);
			}
			else
			{
				++iterator;
			}
		}
	}

	// Step 3)  Refresh transparency for new items
	for (int i = 0; i < MouseHighlightingTiles.size(); i++)
	{
		//if ((App.Vision.hasHumanNationExplored(getTiles().MouseHighlightingTiles[i])) && ((getTiles().Tiles[getTiles().MouseHighlightingTiles[i]].isWaterTile() != true)))
		{
			bool Found = false;
			if (TileTransparencies.size() > 0)
			{
				for (std::list<Tile_Transparency_Tracker>::iterator iterator = TileTransparencies.begin(), end = TileTransparencies.end(); iterator != end; ++iterator)
				{
					if ((*iterator).AssociatedTile == MouseHighlightingTiles[i])
					{
						Found = true;
						(*iterator).CurrentTransparency = 255;
						(*iterator).TimeAdded = CurrentTimeClock;
					}
				}
			}

			if (Found != true)
			{
				Tile_Transparency_Tracker Marker = Tile_Transparency_Tracker();
				Marker.AssociatedTile = MouseHighlightingTiles[i];
				Marker.CurrentTransparency = 255;
				Marker.TimeAdded = CurrentTimeClock;
				TileTransparencies.push_back(Marker);
			}
		}
	}
}

//==================================

void IUEINW::IUEINW_Mouse_Tile_Tracker::findMouseTileIndex()
{
//	if (App.Input.MouseMovementTracker.MouseInsideWindowBounds)
	{
		PreviouslyMousedTile = MouseTileIndex;

		MouseTileIndex = getTiles().Grid.findTileClosestToPoint(fi::getInput().MouseWorldPosition);
		if (MouseTileIndex != -1)
		{
			IsMouseActuallyInsideTile = getTiles().isPointInsideTile(fi::getInput().MouseWorldPosition.x, fi::getInput().MouseWorldPosition.y, MouseTileIndex);
		}
		else
		{
			IsMouseActuallyInsideTile = false;
		}
	}
}