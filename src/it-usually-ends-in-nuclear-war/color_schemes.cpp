#include "color_schemes.h"
#include "map.h"

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Plugin_Init_Color_Schemes::work(const int Event)
{
    getColorSchemes().init();
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Color_Schemes::init()
{
	TileTypeColors.clear();
	UnexploredColors.clear();
	auto VisualSection = fi::getConfig()["visuals"];

	sf::Color MasterTileColorOffset = fi::jsonGetColor(VisualSection, "master-tile-color-offset");
	BarbarianColor = fi::jsonGetColor(VisualSection, "barbarian-color");
	NotificationTextSizeOffset = VisualSection["notification-text-size-offset"].get<int>();
	NotificationTransparencyTime = VisualSection["notification-transparency-time"].get<float>();

    CityQuadOffset = fi::getConfig()["game"]["map"]["city-drawable-tile-offset"].get<int>(); // todo should be in visual section

    TileTypeShadowColorOffset.clear();
    TileTypeNonShadowColorOffset.clear();

	auto ColorSchemesArray = VisualSection["map-color-schemes"];
	TileTypeColors.resize(ColorSchemesArray.size());
	ColorSchemeNames.resize(ColorSchemesArray.size());
	UnexploredColors.resize(ColorSchemesArray.size());
	MapEdgeColors.resize(ColorSchemesArray.size());
	LandOceanBorderLineColors.resize(ColorSchemesArray.size());

	NationBorderThickness.resize(ColorSchemesArray.size());
	NationBorderColorType.resize(ColorSchemesArray.size());
	NationBorderPrimitiveType.resize(ColorSchemesArray.size());

	GridLinesAroundMouseVisionColor.resize(ColorSchemesArray.size());
	GridLinesAroundMouseNonVisionColor.resize(ColorSchemesArray.size());
	GridLinesAlwaysOnVisionColor.resize(ColorSchemesArray.size());
	GridLinesAlwaysOnNonVisionColor.resize(ColorSchemesArray.size());

	BattleLineColors.resize(ColorSchemesArray.size());
	SelectionColors.resize(ColorSchemesArray.size());

	RoadColor.resize(ColorSchemesArray.size());
	RoadThickness.resize(ColorSchemesArray.size());

	AttritionColor.resize(ColorSchemesArray.size());

	SettlerOverlayGridValidColor.resize(ColorSchemesArray.size());;
	SettlerOverlayGridInvalidColor.resize(ColorSchemesArray.size());;
	SettlerOverlayRingValidColor.resize(ColorSchemesArray.size());;
	SettlerOverlayRingInvalidColor.resize(ColorSchemesArray.size());;


	GridLinesAlwaysOnTileGrouping.resize(ColorSchemesArray.size());
	GridLinesAroundMouseTileGrouping.resize(ColorSchemesArray.size());

    UseNationColorInsteadOfExplicitCityColor.resize(ColorSchemesArray.size());
	ExplicitCityColor.resize(ColorSchemesArray.size());
    CityUndersideColor.resize(ColorSchemesArray.size());

	SelectedColorSchemeIndex = 0;
	std::string SelectedColorSchemeName = VisualSection["selected-color-scheme"].get<std::string>();

    int i = 0;
	for (auto it = ColorSchemesArray.begin(); it != ColorSchemesArray.end(); ++it)
	{
		auto SchemeDefinition = (*it);

		ColorSchemeNames[i] = SchemeDefinition["name"].get<std::string>();
		if (ColorSchemeNames[i] == SelectedColorSchemeName)
		{
			SelectedColorSchemeIndex = i;
		}


		TileTypeColors[i].resize(NUMBER_OF_TILE_TYPES + 1);
		TileTypeColors[i][GRASS] = fi::jsonGetColor(SchemeDefinition, "grassland-tile-color");
		TileTypeColors[i][DESERT] = fi::jsonGetColor(SchemeDefinition, "desert-tile-color");
		TileTypeColors[i][FOREST] = fi::jsonGetColor(SchemeDefinition, "forest-tile-color");
		TileTypeColors[i][ICE] = fi::jsonGetColor(SchemeDefinition, "ice-tile-color");
		TileTypeColors[i][PLAINS] = fi::jsonGetColor(SchemeDefinition, "plains-tile-color");
		TileTypeColors[i][OCEAN] = fi::jsonGetColor(SchemeDefinition, "ocean-tile-color");
		TileTypeColors[i][COASTAL_OCEAN] = fi::jsonGetColor(SchemeDefinition, "coastal-tile-color");
		TileTypeColors[i][NUMBER_OF_TILE_TYPES] = fi::jsonGetColor(SchemeDefinition, "small-board-extra-type-tile-color");
		TileTypeShadowColorOffset.push_back(SchemeDefinition["tile-type-shadow-color-offset"].get<int>());
        TileTypeNonShadowColorOffset.push_back(SchemeDefinition["tile-type-non-shadow-color-offset"].get<int>());

		for (unsigned int j = 0; j < TileTypeColors[i].size(); j++)
		{
			TileTypeColors[i][j] = fi::colorModify(TileTypeColors[i][j], MasterTileColorOffset.r, MasterTileColorOffset.g, MasterTileColorOffset.b, MasterTileColorOffset.a);
		}

		UnexploredColors[i] = fi::jsonGetColor(SchemeDefinition, "unexplored");
		UnexploredColors[i].a = 255; // nice try, assholes!

		GridLinesAlwaysOnVisionColor[i] = fi::jsonGetColor(SchemeDefinition, "always-on-grid-line-vision-color");
		GridLinesAlwaysOnNonVisionColor[i] = fi::jsonGetColor(SchemeDefinition, "always-on-grid-line-nonvision-color");
		GridLinesAroundMouseVisionColor[i] = fi::jsonGetColor(SchemeDefinition, "grid-lines-around-mouse-vision-color");
		GridLinesAroundMouseNonVisionColor[i] = fi::jsonGetColor(SchemeDefinition, "grid-lines-around-mouse-nonvision-color");

		MapEdgeColors[i].push_back(fi::jsonGetColor(SchemeDefinition, "front-dirt-color-left"));
		MapEdgeColors[i].push_back(fi::jsonGetColor(SchemeDefinition, "front-dirt-color-right"));
		MapEdgeColors[i].push_back(fi::jsonGetColor(SchemeDefinition, "map-edge-line-color"));

		LandOceanBorderLineColors[i] = fi::jsonGetColor(SchemeDefinition, "land-ocean-border-line-color");

		AttritionColor[i] = fi::jsonGetColor(SchemeDefinition, "attrition-color");
		BattleLineColors[i] = fi::jsonGetColor(SchemeDefinition, "battle-line-color");
		SelectionColors[i] = fi::jsonGetColor(SchemeDefinition, "selection-color");

		RoadColor[i] = fi::jsonGetColor(SchemeDefinition, "road-color");
		RoadThickness[i] = SchemeDefinition["road-thickness"].get<int>();

		SettlerOverlayGridValidColor[i] = fi::jsonGetColor(SchemeDefinition, "settler-overlay-grid-valid-color");
		SettlerOverlayGridInvalidColor[i] = fi::jsonGetColor(SchemeDefinition, "settler-overlay-grid-invalid-color");
		SettlerOverlayRingValidColor[i] = fi::jsonGetColor(SchemeDefinition, "settler-overlay-ring-valid-color");
		SettlerOverlayRingInvalidColor[i] = fi::jsonGetColor(SchemeDefinition, "settler-overlay-ring-invalid-color");

		if (SchemeDefinition["always-on-grid-line-tile-grouping"].get<std::string>() == "all")
		{
			GridLinesAlwaysOnTileGrouping[i] = Tile_Type_Grouping::ALL_TILE_TYPES;
		}
		if (SchemeDefinition["always-on-grid-line-tile-grouping"].get<std::string>() == "ocean")
		{
			GridLinesAlwaysOnTileGrouping[i] = Tile_Type_Grouping::OCEAN_TILE_TYPES;
		}
		if (SchemeDefinition["always-on-grid-line-tile-grouping"].get<std::string>() == "land")
		{
			GridLinesAlwaysOnTileGrouping[i] = Tile_Type_Grouping::LAND_TILE_TYPES;
		}
		if (SchemeDefinition["always-on-grid-line-tile-grouping"].get<std::string>() == "none")
		{
			GridLinesAlwaysOnTileGrouping[i] = Tile_Type_Grouping::NONE;
		}

		if (SchemeDefinition["grid-lines-around-mouse-tile-grouping"].get<std::string>() == "all")
		{
			GridLinesAroundMouseTileGrouping[i] = Tile_Type_Grouping::ALL_TILE_TYPES;
		}
		if (SchemeDefinition["grid-lines-around-mouse-tile-grouping"].get<std::string>() == "ocean")
		{
			GridLinesAroundMouseTileGrouping[i] = Tile_Type_Grouping::OCEAN_TILE_TYPES;
		}
		if (SchemeDefinition["grid-lines-around-mouse-tile-grouping"].get<std::string>() == "land")
		{
			GridLinesAroundMouseTileGrouping[i] = Tile_Type_Grouping::LAND_TILE_TYPES;
		}
		if (SchemeDefinition["grid-lines-around-mouse-tile-grouping"].get<std::string>() == "none")
		{
			GridLinesAroundMouseTileGrouping[i] = Tile_Type_Grouping::NONE;
		}


		NationBorderThickness[i] = SchemeDefinition["nation-border-thickness"].get<int>();
		NationBorderColorType[i] = fi::stringDowncase(SchemeDefinition["nation-border-color-type"].get<std::string>());
		NationBorderPrimitiveType[i] = fi::stringDowncase(SchemeDefinition["nation-border-vertex-array-primitive-type"].get<std::string>());

        UseNationColorInsteadOfExplicitCityColor[i] = SchemeDefinition["use-nation-color-instead-of-explicit-city-color"].get<bool>();
        ExplicitCityColor[i] = fi::jsonGetColor(SchemeDefinition, "explicit-city-color");
		CityUndersideColor[i] = fi::jsonGetColor(SchemeDefinition, "city-underside-color");

		i++;
	}

	// ----

	NationColorSchemes.clear();

	auto VisualsJSON = fi::getConfig()["visuals"];
	auto ColorSchemeJSONArray = VisualsJSON["nation-color-schemes"];

	for (auto it = ColorSchemeJSONArray.begin(); it != ColorSchemeJSONArray.end(); ++it)
	{
		std::string HexString = (*it).get<std::string>();

		sf::Color Color;
		fi::colorConvertToRGB(HexString, Color);
		NationColorSchemes.push_back(Color);
	}

	// todo -- does not work on windows due to being deprecated
	//std::random_shuffle(std::begin(NationColorSchemes), NationColorSchemes.end());

	fi::getEngine().setBackgroundColor(getUnexplored());
}