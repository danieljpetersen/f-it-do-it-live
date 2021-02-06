#ifndef IUEINW_MAP_H
#define IUEINW_MAP_H

#include "../common/engine.h"
#include "tile_types.h"
#include "map_size.h"
#include "globals.h"

namespace IUEINW
{
    const int CITY_CREATION_REVEAL_AREA_SIZE = 11;

    // ----

    class IUEINW_Tile
	{
	public:
	    sf::Color DebugColor;//todo delete

		int TileIndex;
		int Height[4];

		sf::Color aColor, bColor;
		bool VerticalSplit = false;
		bool HorizontalSplit = false;
		bool isFlat()
		{
			return VerticalSplit == false && HorizontalSplit == false;
		}

		int TileType, NationIndex, CityIndex;
		int ContinentIndex, BodyOfWaterIndex;
		int ProductionOutputBase, FoodOutputBase, OutputBonusBase;
		bool IsCityTile = false; // CityIndex indicates what city it belongs to, this variable indicates whether this is the actual tile containing the city
        float DistanceFromMousedTile = 0;

		bool isWaterTile() { return ((TileType == OCEAN) || (TileType == COASTAL_OCEAN)); }
		int calculateLossFromAttrition(int NationIndex);

		template <class Archive>
		void serialize( Archive & ar )
		{
			ar(TileIndex);
			ar(TileType);
			ar(Height[0]);
			ar(Height[1]);
			ar(Height[2]);
			ar(Height[3]);
			ar(aColor);
			ar(bColor);
			ar(NationIndex);
			ar(CityIndex);
			ar(ContinentIndex);
			ar(ProductionOutputBase);
			ar(FoodOutputBase);
			ar(OutputBonusBase);
			ar(VerticalSplit);
			ar(HorizontalSplit);
		}
	};

	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////

	class IUEINW_Tiles
	{
	public:
		void init(int ColCount, int RowCount);

		template <class Archive>
		void serialize( Archive & ar )
		{
			ar(Grid);
			ar(LatitudeZone_NumberOfRowsInTopAndBottom);
			ar(MAX_CACHE_AREA);
			ar(Y_TILE_SLOPE_OFFSET);
		}

		fi::Grid_1D_Custom_Data<IUEINW_Tile> Grid;
		std::vector<std::vector<int>> EdgeTiles;
		std::vector<int> LandTiles, WaterTiles, AllTiles;
		std::vector<std::vector<int>> BodiesOfWater, Continents;

		IUEINW_Tile   operator [](int i) const {return Grid.CustomCellData[i];}
		IUEINW_Tile & operator [](int i)       {return Grid.CustomCellData[i];}
		int size(){ return Grid.size(); }

		bool isPointInsideTile(int x, int y, int Index);
		bool areSameContinent(int aTileIndex, int bTileIndex);

        int getRandomLandTile();
        int getRandomLandTile(int ContinentIndex);
		static void collectTilesInColumn_WithoutWaterTiles(int ColIndex, int StartingRow, int EndingRow, std::vector<int> &TileCollection);
		static void collectTilesInColumn(int ColIndex, int StartingRow, int EndingRow, std::vector<int> &TileCollection);
		std::vector<int> *getArea(int Center, int HalfSize, Tile_Type_Grouping TileTypeGrouping, int &Size);
		std::vector<int> getAreaSpreadingFromCenter(int Start, int DistanceCutoff, bool WithOceanTiles);

		//RETURNS INDEX FOR CoT [CoT.getTile(result)] --  In other words, use return CoT.getTile(result) to get the actual Tiles.Tiles index
		int findClosestTileFromTileIndex_returnsLocalIndex(std::vector<int> &TileList, int GoalTile);
		int findClosestTileFromTileIndex_returnsLocalIndex(std::vector<int> &TileList, int GoalTile, bool IncludeWaterTilesInSearch);

		std::string convertTileTypeToString(int TileType);
		int convertTileTypeStringToInt(std::string TileType);

	private:
		void populateAreaCache();
		void determineContinents();
		void determineBodiesOfWater();
		void determineLandAndWaterTiles();
		void offsetTileCoordinatesFromHeight();

		int LatitudeZone_NumberOfRowsInTopAndBottom = 0;

		int MAX_CACHE_AREA = -1;
		float Y_TILE_SLOPE_OFFSET = 0;
		std::vector<std::vector<int>> AreaCache_All, AreaCache_Land, AreaCache_Water;
		std::vector<std::vector<int>> AreaCacheIndexMap_All, AreaCacheIndexMap_Land, AreaCacheIndexMap_Water;

		std::vector<int> calculateArea_ImplementationOne(int Center, int NumberOfRings, bool WithWaterTiles); // order is different -- goes column by column
		std::vector<int> calculateArea_ImplementationTwo(int Center, int NumberOfRings, Tile_Type_Grouping TileTypeGrouping); // order is different -- goes in rings

		friend class IUEINW_Plugin_Generate_Map;
		friend class IUEINW_Map;
	};

	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////

    class IUEINW_Map
	{
	public:
		IUEINW_Tiles Tiles;
		IUEINW_Tile_Type_Properties TileTypeProperties;
		IUEINW_Map_Layout CurrentMapLayout;
        IUEINW_Map_Layouts PossibleMapLayouts;

		//template <class Archive>
		//void serialize( Archive & ar )
		//{
		//	ar(Tiles);
		//	ar(PossibleMapLayouts);
		//	ar(TileTypeProperties);
		//	ar(CurrentMapLayout);
		//}
        //
		//void afterLoad() override
		//{
		//	Tiles.Grid.ColumnTransforms.clear();
		//	Tiles.Grid.ColumnTransforms.resize(Tiles.Grid.NumberOfColumns);
        //
		//	Tiles.populateAreaCache();
		//	Tiles.determineContinents();
		//	Tiles.determineBodiesOfWater();
		//	Tiles.determineLandAndWaterTiles();
		//}

		//FI_MODULE_SERIALIZATION_BOILERPLATE
	};

    ////////////////////////////////////////////////////////////

    IUEINW_Map &getMap();
    IUEINW_Tiles &getTiles();
    fi::Grid_1D_Custom_Data<IUEINW_Tile> &getGrid();
}

#endif