#ifndef TILE_SHADOWING_H
#define TILE_SHADOWING_H

#include <SFML/Graphics.hpp>
#include "../common/engine.h"
#include "tile_types.h"

namespace IUEINW
{
    class IUEINW_Color_Schemes : public fi::Plugin_Base
	{
	public:
		void onProgramStart() override;

		int getNotificationTextSizeOffset()
		{
			return NotificationTextSizeOffset;
		}
		float getNotificationTransparencyTime()
		{
			return NotificationTransparencyTime;
		}

		sf::Color getBarbarianColor()
		{
			return BarbarianColor;
		}

		sf::Color getUnexplored()
		{
			return UnexploredColors[SelectedColorSchemeIndex];
		}

		sf::Color getTileColor(int TileType)
		{
			return TileTypeColors[SelectedColorSchemeIndex][TileType];
		}

		sf::Color calculateLighterColor_Shadow(sf::Color Color)
		{
			return fi::colorModify(Color, TileTypeShadowColorOffset[SelectedColorSchemeIndex], TileTypeShadowColorOffset[SelectedColorSchemeIndex], 0, 0);
		}

		sf::Color calculateDarkerColor_Shadow(sf::Color Color)
		{
			return fi::colorModify(Color, -TileTypeShadowColorOffset[SelectedColorSchemeIndex], -TileTypeShadowColorOffset[SelectedColorSchemeIndex], 0, 0);
		}

		sf::Color calculateLighterColor_NonShadow(sf::Color Color)
		{
			return fi::colorModify(Color, TileTypeNonShadowColorOffset[SelectedColorSchemeIndex], TileTypeNonShadowColorOffset[SelectedColorSchemeIndex], 0, 0);
		}

		sf::Color calculateDarkerColor_NonShadow(sf::Color Color)
		{
			return fi::colorModify(Color, -TileTypeNonShadowColorOffset[SelectedColorSchemeIndex], -TileTypeNonShadowColorOffset[SelectedColorSchemeIndex], 0, 0);
		}

		sf::Color getLeftFrontDirtColor()
		{
			return MapEdgeColors[SelectedColorSchemeIndex][0];
		}

		sf::Color getRightFrontDirtColor()
		{
			return MapEdgeColors[SelectedColorSchemeIndex][1];
		}

		sf::Color getMapEdgeLineColor()
		{
			return MapEdgeColors[SelectedColorSchemeIndex][2];
		}

		sf::Color getLandOceanBorderLineColor()
		{
			return LandOceanBorderLineColors[SelectedColorSchemeIndex];
		}

		sf::Color getGridLinesAroundMouseVisionColor()
		{
			return GridLinesAroundMouseVisionColor[SelectedColorSchemeIndex];
		}

		sf::Color getGridLinesAroundMouseNonVisionColor()
		{
			return GridLinesAroundMouseNonVisionColor[SelectedColorSchemeIndex];
		}

		sf::Color getGridLinesAlwaysOnVisionColor()
		{
			return GridLinesAlwaysOnVisionColor[SelectedColorSchemeIndex];
		}

		sf::Color getGridLinesAlwaysOnNonVisionColor()
		{
			return GridLinesAlwaysOnNonVisionColor[SelectedColorSchemeIndex];
		}

		sf::Color getAttritionColor()
		{
			return AttritionColor[SelectedColorSchemeIndex];
		}

		sf::Color getSettlerOverlayGridValidColor()
		{
			return SettlerOverlayGridValidColor[SelectedColorSchemeIndex];
		}

		sf::Color getSettlerOverlayGridInvalidColor()
		{
			return SettlerOverlayGridInvalidColor[SelectedColorSchemeIndex];
		}

		sf::Color getSettlerOverlaRingInvalidColor()
		{
			return SettlerOverlayRingInvalidColor[SelectedColorSchemeIndex];
		}

		sf::Color getSettlerOverlayRingValidColor()
		{
			return SettlerOverlayRingValidColor[SelectedColorSchemeIndex];
		}

		sf::Color getBattleLineColor()
		{
			return BattleLineColors[SelectedColorSchemeIndex];
		}

		sf::Color getSelectionColor()
		{
			return SelectionColors[SelectedColorSchemeIndex];
		}

		sf::Color getRoadColor()
		{
			return RoadColor[SelectedColorSchemeIndex];
		}

		int getRoadThickness()
		{
			return RoadThickness[SelectedColorSchemeIndex];
		}

		Tile_Type_Grouping getGridLinesAlwaysOnTileGrouping()
		{
			return GridLinesAlwaysOnTileGrouping[SelectedColorSchemeIndex];
		}

		Tile_Type_Grouping getGridLinesAroundMouseTileGrouping()
		{
			return GridLinesAroundMouseTileGrouping[SelectedColorSchemeIndex];
		}

		int getNationBorderThickness()
		{
			return NationBorderThickness[SelectedColorSchemeIndex];
		}

		std::string getNationBorderColorType()
		{
			return NationBorderColorType[SelectedColorSchemeIndex];
		}

		std::string getNationBorderPrimitiveType()
		{
			return NationBorderPrimitiveType[SelectedColorSchemeIndex];
		}

		sf::Color getExplicitCityColor()
		{
			return ExplicitCityColor[SelectedColorSchemeIndex];
		}

		sf::Color getGoodyHutColor()
		{
			return GoodyHutColor[SelectedColorSchemeIndex];
		}

		sf::Color getNationColor(int Index)
		{
			if (Index < NationColorSchemes.size())
			{
				return NationColorSchemes[Index];
			}

			return fi::getRandom().color();
		}

	private:
		// Coastal is the last index
		std::vector<std::vector<sf::Color>> TileTypeColors;
		std::vector<sf::Color> UnexploredColors;
		std::vector<std::string> ColorSchemeNames;
		std::vector<int> TileTypeShadowColorOffset;
		std::vector<int> TileTypeNonShadowColorOffset;
		int SelectedColorSchemeIndex;

		std::vector<sf::Color> LandOceanBorderLineColors;
		std::vector<std::vector<sf::Color>> MapEdgeColors;

		std::vector<sf::Color> GridLinesAroundMouseVisionColor, GridLinesAroundMouseNonVisionColor;
		std::vector<sf::Color> GridLinesAlwaysOnVisionColor, GridLinesAlwaysOnNonVisionColor;
		std::vector<sf::Color> BattleLineColors;
		std::vector<sf::Color> SelectionColors;
		std::vector<sf::Color> AttritionColor;
		std::vector<Tile_Type_Grouping> GridLinesAlwaysOnTileGrouping;
		std::vector<Tile_Type_Grouping> GridLinesAroundMouseTileGrouping;

		std::vector<sf::Color> NationColorSchemes;

		std::vector<sf::Color> RoadColor;
		std::vector<int> RoadThickness;

		std::vector<sf::Color> SettlerOverlayGridValidColor, SettlerOverlayGridInvalidColor;
		std::vector<sf::Color> SettlerOverlayRingValidColor, SettlerOverlayRingInvalidColor;

		std::vector<int> NationBorderThickness;
		std::vector<std::string> NationBorderPrimitiveType;
		std::vector<std::string> NationBorderColorType;

		std::vector<sf::Color> ExplicitCityColor;
		std::vector<sf::Color> GoodyHutColor;

		sf::Color BarbarianColor;

		float NotificationTransparencyTime;
		int NotificationTextSizeOffset;
	};

	////////////////////////////////////////////////////////////

	IUEINW_Color_Schemes &getColorSchemes();

}

#endif 