#include "name_tags.h"
#include "units.h"
#include "cities.h"
#include "map.h"
#include "vision.h"

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Plugin_Name_Tags::addUnitNameTags()
{
    auto Units = getUnits().Units.ReadPtr;
    for (int i = 0; i < Units->size(); i++)
    {
        int TileIndex = Units->Objects[i].TileIndex;

        if (getVision().hasVision(TileIndex))
        {
            NameTags[TileIndex].Strength += Units->Objects[i].Strength;
        }
    }
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Plugin_Name_Tags::addCityNameTags()
{
    auto Cities = getCities().Cities.ReadPtr;
    for (int i = 0; i < Cities->size(); i++)
    {
    	auto City = &Cities->at(i);
		if (City->NationIndex != -1)
		{
			if (getVision().hasVision(City->TileIndex))
			{
				int TileIndex = City->TileIndex;
				NameTags[TileIndex].HasCity = true;
			}
		}
    }
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Plugin_Name_Tags::renderNameTags()
{
    for (auto NameTag : NameTags)
    {
    	if (NameTag.second.HasCity != true)
		{
			auto TileCenter = getGrid().CommonCellData[NameTag.first].Center;
			auto WindowCoordinates = fi::getCanvasWorld().mapCoordsToPixel(TileCenter);
			fi::guiTextCenter(WindowCoordinates.x,WindowCoordinates.y, fi::to_s(NameTag.second.Strength));
		}
    }
}

////////////////////////////////////////////////////////////
