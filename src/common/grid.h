#ifndef FI_GRID_H
#define FI_GRID_H

#include <vector>
#include <SFML/System.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <sstream>
#include <climits>
#include "util.h" // for cereal conversion
#include "draw.h"
#include "fi_math.h"

////////////////////////////////////////////////////////////
// some constants. Don't change the ordering of these, you fuck.
// yes, this is garbage, and yes, this is all very redundant

const int NO_EDGE = -1;
const int BOTTOMLEFT_EDGE = 0;
const int LEFT_EDGE = 0;
const int BOTTOMRIGHT_EDGE = 1;
const int BOTTOM_EDGE = 1;
const int TOPLEFT_EDGE = 2;
const int TOP_EDGE = 2;
const int TOPRIGHT_EDGE = 3;
const int RIGHT_EDGE = 3;
const int LEFTCORNER_EDGE = 4;
const int TOPLEFTCORNER_EDGE = 4;
const int RIGHTCORNER_EDGE = 5;
const int BOTTOMRIGHTCORNER_EDGE = 5;
const int TOPCORNER_EDGE = 6;
const int TOPRIGHTCORNER_EDGE = 6;
const int BOTTOMCORNER_EDGE = 7;
const int BOTTOMLEFTCORNER_EDGE = 7;

const int TOPLEFT = 0;
const int TOPRIGHT = 1;
const int BOTTOMLEFT = 2;
const int BOTTOMRIGHT = 3;

const int TileToLeft = 0;
const int TileToTopLeft = 1;
const int TileToTop = 2;
const int TileToTopRight = 3;
const int TileToRight = 4;
const int TileToBottomRight = 5;
const int TileToBottom = 6;
const int TileToBottomLeft = 7;
const int NumberOfTileNeighbors = 8;

const int LEFT = 0;
const int TOP = 1;
const int BACK = 1;
const int RIGHT = 2;
const int BOTTOM = 3;
const int FRONT = 3;

///////////////////////////////////////////////////////////

namespace fi
{

    const int GRID_TYPE_ISOMETRIC = 0;
    const int GRID_TYPE_SQUARE = 1;
    const int GRID_TYPE_HEX = 2;

    // ----

    struct Grid_Common_Cell_Data
	{
		sf::Vector2f Center;

		std::vector<sf::Vector2f> Corners;

		int Edge, Column, Row;
		int Height, Width;
		
		template <class Archive>
		void serialize( Archive & ar )
		{
			ar(Center.x);
			ar(Center.y);
			ar(Corners);
			ar(Column);
			ar(Row);
			ar(Height);
			ar(Width);
		}
		
		sf::Vector2f getLeft2f()
		{
			return Corners[LEFT];
		}
		sf::Vector2f getBack2f()
		{
			return Corners[BACK];
		}
		sf::Vector2f getRight2f()
		{
			return Corners[RIGHT];
		}
		sf::Vector2f getFront2f()
		{
			return Corners[FRONT];
		}
	};

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

	class Grid_Base
	{
	public:
		// contains vertex data
		std::vector<Grid_Common_Cell_Data> CommonCellData;
		std::vector<sf::Transform> ColumnTransforms;// todo port over horizontal wrap code

		// Number of Points on cell vertex array
		int CornerSize;

		// we store the neighbor information for faster lookup
		std::vector<std::vector<int>> Neighbors;

		// iteration is ordered as follows:  for (x = 0; x < ColCount; x++) { for (y = 0; y < rowCount; y++) }
		// it was ordered this way to allow for round-world wraps (easier slicing of array if we know we just want to translate the first n tiles)
		int NumberOfColumns, NumberOfRows, NumberOfCells, HalfColumnCount, NumberOfNeighbors, CellSpacing, LastColumn, LastRow;

		// GRID_TYPE_ISOMETRIC || GRID_TYPE_SQUARE || GRID_TYPE_HEX
		int GridType, TileHeight, TileWidth;

		bool HorizontalWrap; // todo port over horizontal wrap code

	////////////////////////////////////////////////////////////

		virtual void init(int NumberOfRows, int NumberOfColumns, int GridType, int CellSpacing, int CellWidth, int CellHeight, bool HorizontalWrap)
		{
			this->NumberOfRows = NumberOfRows;
			this->NumberOfColumns = NumberOfColumns;
			this->NumberOfCells = NumberOfRows * NumberOfColumns;
			this->HalfColumnCount = NumberOfColumns/2;
			this->GridType = GridType;
			this->CellSpacing = CellSpacing;
			this->TileWidth = CellWidth;
			this->TileHeight = CellHeight;
			this->LastColumn = NumberOfColumns - 1;
			this->LastRow = NumberOfRows - 1;
			this->HorizontalWrap = HorizontalWrap;

            if (GridType == GRID_TYPE_ISOMETRIC)
            {
                this->TileHeight = this->TileHeight / 2;
            }

			CommonCellData.clear();
			CommonCellData.resize(NumberOfCells);
			ColumnTransforms.clear();
			ColumnTransforms.resize(NumberOfColumns);
			Neighbors.clear();

			_initCommonCellData(CellWidth, CellHeight);
		}

	////////////////////////////////////////////////////////////

		virtual void resizeCustomCellData(int NumberOfCells) { };

	////////////////////////////////////////////////////////////

		int size()
		{
			return NumberOfCells;
		}

	////////////////////////////////////////////////////////////
		
		int getRow(int __CellIndex)
		{
			return CommonCellData[__CellIndex].Row;
		}
		
	////////////////////////////////////////////////////////////
	
		int getCol(int __CellIndex)
		{
			return CommonCellData[__CellIndex].Column;
		}
		
		////////////////////////////////////////////////////////////
		
		int getCellIndex(int __Col, int __Row)
		{
			//return (NumberOfColumns * Col) + Row; // for when grid goes left to right, top to bototm
			return (NumberOfRows * __Col) + __Row; 	    // for when grid goes top to bottom, left to right
		}

	////////////////////////////////////////////////////////////

		int getCellIndex_ValidateCoordinates(int __Col, int __Row)
		{
			if (__Col < 0)
				return -1;
			if (__Col >= NumberOfColumns)
				return -1;

			if (__Row < 0)
				return -1;
			if (__Row >= NumberOfRows)
				return -1;

			return getCellIndex(__Col, __Row);
		}

	////////////////////////////////////////////////////////////

		bool isValidTile(int Index)
		{
			if (Index < 0)
				return false;

			if (Index >= NumberOfCells)
				return false;

			return true;
		}

	////////////////////////////////////////////////////////////

		bool isValidCell(int Index)
		{
			if (Index < 0)
			{
				return false;
			}

			if (Index >= NumberOfCells)
			{
				return false;
			}

			return true;
		}

	////////////////////////////////////////////////////////////

		int getFrontTile()
		{
			return getCellIndex(0, NumberOfRows-1);
		}

	////////////////////////////////////////////////////////////

		int getRightTile()
		{
			return getCellIndex(NumberOfColumns-1, NumberOfRows-1);
		}

	////////////////////////////////////////////////////////////

		int getBackTile()
		{
			return getCellIndex(NumberOfColumns-1, 0);
		}

	////////////////////////////////////////////////////////////

		int getLeftTile()
		{
			return 0;
		}

	////////////////////////////////////////////////////////////

		void buildVertexForCell_Triangles(int TileIndex, sf::Color Color, sf::VertexArray &VertArray)
		{
			sf::Vertex LeftPoint   = sf::Vertex(CommonCellData[TileIndex].Corners[LEFT]);
			sf::Vertex TopPoint    = sf::Vertex(CommonCellData[TileIndex].Corners[BACK]);
			sf::Vertex RightPoint  = sf::Vertex(CommonCellData[TileIndex].Corners[RIGHT]);
			sf::Vertex BottomPoint = sf::Vertex(CommonCellData[TileIndex].Corners[FRONT]);

			LeftPoint.color   = Color;
			RightPoint.color  = Color;
			TopPoint.color    = Color;
			BottomPoint.color = Color;

			VertArray.append(RightPoint);
			VertArray.append(TopPoint);
			VertArray.append(LeftPoint);

			VertArray.append(RightPoint);
			VertArray.append(BottomPoint);
			VertArray.append(LeftPoint);
		}

		void buildVertexForCell_Triangles(int TileIndex, sf::Color Color, std::vector<sf::Vertex> &VertArray)
		{
			sf::Vertex LeftPoint   = sf::Vertex(CommonCellData[TileIndex].Corners[LEFT]);
			sf::Vertex TopPoint    = sf::Vertex(CommonCellData[TileIndex].Corners[BACK]);
			sf::Vertex RightPoint  = sf::Vertex(CommonCellData[TileIndex].Corners[RIGHT]);
			sf::Vertex BottomPoint = sf::Vertex(CommonCellData[TileIndex].Corners[FRONT]);

			LeftPoint.color   = Color;
			RightPoint.color  = Color;
			TopPoint.color    = Color;
			BottomPoint.color = Color;

			VertArray.push_back(RightPoint);
			VertArray.push_back(TopPoint);
			VertArray.push_back(LeftPoint);

			VertArray.push_back(RightPoint);
			VertArray.push_back(BottomPoint);
			VertArray.push_back(LeftPoint);
		}

	////////////////////////////////////////////////////////////

		void buildVertexForCell_HexTriangles(int TileIndex, sf::Color Color, sf::VertexArray &VertArray)
		{
			sf::Vertex Triangle[3];
			Triangle[0].color = Color;
			Triangle[1].color = Color;
			Triangle[2].color = Color;

			Triangle[0].position = CommonCellData[TileIndex].Center;
			Triangle[1].position = CommonCellData[TileIndex].Corners[0];
			Triangle[2].position = CommonCellData[TileIndex].Corners[1];
			VertArray.append(Triangle[0]);
			VertArray.append(Triangle[1]);
			VertArray.append(Triangle[2]);

			Triangle[1].position = CommonCellData[TileIndex].Corners[0];
			Triangle[2].position = CommonCellData[TileIndex].Corners[2];
			VertArray.append(Triangle[0]);
			VertArray.append(Triangle[1]);
			VertArray.append(Triangle[2]);

			Triangle[1].position = CommonCellData[TileIndex].Corners[2];
			Triangle[2].position = CommonCellData[TileIndex].Corners[4];
			VertArray.append(Triangle[0]);
			VertArray.append(Triangle[1]);
			VertArray.append(Triangle[2]);

			Triangle[1].position = CommonCellData[TileIndex].Corners[4];
			Triangle[2].position = CommonCellData[TileIndex].Corners[3];
			VertArray.append(Triangle[0]);
			VertArray.append(Triangle[1]);
			VertArray.append(Triangle[2]);

			Triangle[1].position = CommonCellData[TileIndex].Corners[5];
			Triangle[2].position = CommonCellData[TileIndex].Corners[3];
			VertArray.append(Triangle[0]);
			VertArray.append(Triangle[1]);
			VertArray.append(Triangle[2]);

			Triangle[1].position = CommonCellData[TileIndex].Corners[5];
			Triangle[2].position = CommonCellData[TileIndex].Corners[1];
			VertArray.append(Triangle[0]);
			VertArray.append(Triangle[1]);
			VertArray.append(Triangle[2]);
		}

	////////////////////////////////////////////////////////////

		void buildVertexForCell_LinesWithThickness_PrimitiveTypeQuad(int TileIndex, sf::Color Color, sf::VertexArray &VertArray, int LineThickness)
		{
			sf::Vector2f Point1 = CommonCellData[TileIndex].getLeft2f();
			sf::Vector2f Point2 = CommonCellData[TileIndex].getBack2f();
			sf::Vector2f Point3 = CommonCellData[TileIndex].getRight2f();
			sf::Vector2f Point4 = CommonCellData[TileIndex].getFront2f();

			fi::Draw::line(VertArray, Point1, Point2, Color, LineThickness);
			fi::Draw::line(VertArray, Point2, Point3, Color, LineThickness);
			fi::Draw::line(VertArray, Point3, Point4, Color, LineThickness);
			fi::Draw::line(VertArray, Point1, Point4, Color, LineThickness);
		}

	////////////////////////////////////////////////////////////

		void buildVertexForCell_Lines(int TileIndex, sf::Color Color, sf::VertexArray &VertArray)
		{
			sf::Vector2f Point1 = CommonCellData[TileIndex].getLeft2f();
			sf::Vector2f Point2 = CommonCellData[TileIndex].getBack2f();
			sf::Vector2f Point3 = CommonCellData[TileIndex].getRight2f();
			sf::Vector2f Point4 = CommonCellData[TileIndex].getFront2f();

			sf::Vertex Vert[2] =
            {
                sf::Vertex(Point1, Color),
                sf::Vertex(Point2, Color)
            };

			VertArray.append(Vert[0]);
			VertArray.append(Vert[1]);

			Vert[0].position = Point2;
			Vert[1].position = Point3;

			VertArray.append(Vert[0]);
			VertArray.append(Vert[1]);

			Vert[0].position = Point3;
			Vert[1].position = Point4;

			VertArray.append(Vert[0]);
			VertArray.append(Vert[1]);

			Vert[0].position = Point4;
			Vert[1].position = Point1;

			VertArray.append(Vert[0]);
			VertArray.append(Vert[1]);
		}

        ////////////////////////////////////////////////////////////

        void buildVertexForCell_Quad(int TileIndex, sf::Color Color, sf::VertexArray &VertArray)
        {
            for (int iter = 0; iter < CornerSize; iter++)
            {
                VertArray.append(sf::Vertex(CommonCellData[TileIndex].Corners[iter], Color));
            }
        }

        ////////////////////////////////////////////////////////////

        void buildVertexForCell_QuadOffset(int TileIndex, sf::Color Color, int Offset, sf::VertexArray &VertArray)
        {
            sf::Vertex Vert(CommonCellData[TileIndex].getLeft2f(), Color);
            Vert.position.x += Offset;
            VertArray.append(Vert);

            Vert.position = CommonCellData[TileIndex].getBack2f();
            Vert.position.y += Offset;
            VertArray.append(Vert);

            Vert.position = CommonCellData[TileIndex].getRight2f();
            Vert.position.x -= Offset;
            VertArray.append(Vert);

            Vert.position = CommonCellData[TileIndex].getFront2f();
            Vert.position.y -= Offset;
            VertArray.append(Vert);
        }

        ////////////////////////////////////////////////////////////

        void updateCenterPositions()
        {
            for (int CellIndex = 0; CellIndex < CommonCellData.size(); CellIndex++)
            {
                int xLeft = CommonCellData[CellIndex].Corners[LEFT].x;
                int xRight = CommonCellData[CellIndex].Corners[RIGHT].x;
                int xCenter = xLeft + (xRight - xLeft)/2;

                int yTop = CommonCellData[CellIndex].Corners[TOP].y;
                int yBottom = CommonCellData[CellIndex].Corners[BOTTOM].y;
                int yCenter = yTop + (yBottom - yTop)/2;

                CommonCellData[CellIndex].Center = sf::Vector2f(xCenter, yCenter);
            }
        }

        ////////////////////////////////////////////////////////////

		int getDistanceBetweenTiles(int aTile, int bTile)
		{
			int ColValue = CommonCellData[bTile].Column - CommonCellData[aTile].Column;

			if (HorizontalWrap)
			{
				const int absColValue = abs(ColValue);
				if (absColValue > HalfColumnCount)
				{
					ColValue = NumberOfColumns - absColValue;
				}
			}

			return hypot(ColValue, CommonCellData[bTile].Row - CommonCellData[aTile].Row);
		}

	////////////////////////////////////////////////////////////
	    // todo performance...
		int findTileClosestToPoint(sf::Vector2i Point)
		{
			int ClosestTileIndex = 0;
			float ClosestDistance = fi::Math::distance(CommonCellData[ClosestTileIndex].Center, convert(Point));

			for (unsigned int i = 0; i < NumberOfCells; i++)
			{
				sf::Vector2f TileCenter = CommonCellData[i].Center;

				float Distance = fi::Math::distance(TileCenter, convert(Point));
				if (Distance < ClosestDistance)
				{
					ClosestDistance = Distance;
					ClosestTileIndex = i;
				}
			}

			return ClosestTileIndex;
		}

	////////////////////////////////////////////////////////////

		int getTileNeighbor(int CenterTile, int WhichNeighbor)
		{
			int Col = CommonCellData[CenterTile].Column;
			const int Row = CommonCellData[CenterTile].Row;

			switch (WhichNeighbor)
            {
				case (TileToLeft):
				{
					if (Col != 0)
					{
						return getCellIndex(Col-1, Row);
					}

					if (HorizontalWrap)
					{
						return getCellIndex(LastColumn, Row);
					}

					break;
				}

				case (TileToTopLeft):
				{
					if (Col == 0)
					{
						if (HorizontalWrap)
						{
							Col = NumberOfColumns;
						}
						else
						{
							break;
						}
					}

					if (Row != 0)
					{
						return getCellIndex(Col-1, Row-1);
					}

					break;
				}

				case (TileToTop):
				{
					if (Row != 0)
					{
						return getCellIndex(Col, Row-1);
					}
					break;
				}

				case (TileToTopRight):
				{
					if (Row == 0)
					{
						break;
					}

					if (Col == LastColumn)
					{
						if (HorizontalWrap)
						{
							Col = -1;
						}
						else
						{
							break;
						}
					}

					return getCellIndex(Col+1, Row-1);
				}

				case (TileToRight):
				{
					if (Col == LastColumn)
					{
						if (HorizontalWrap)
						{
							Col = -1;
						}
						else
						{
							break;
						}
					}

					return getCellIndex(Col+1, Row);
				}

				case (TileToBottomRight):
				{
					if (Row == LastRow)
					{
						break;
					}

					if (Col == LastColumn)
					{
						if (HorizontalWrap)
						{
							Col = -1;
						}
						else
						{
							break;
						}
					}

					return getCellIndex(Col+1, Row+1);
				}

				case (TileToBottom):
				{
					if (Row == LastRow)
					{
						break;
					}

					return getCellIndex(Col, Row + 1);
				}

				case (TileToBottomLeft):
				{
					if (Row == LastRow)
					{
						break;
					}

					if (Col == 0)
					{
						if (HorizontalWrap)
						{
							Col = NumberOfColumns;
						}
						else
						{
							break;
						}
					}
					return getCellIndex(Col-1, Row+1);
				}
			}

			return -1;
		}

	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////

	private:
		void _initCommonCellData(int CellWidth, int CellHeight)
		{
			resizeCustomCellData(NumberOfCells);
			const int CONST_NUMBER = 37;

			if (GridType == GRID_TYPE_HEX)
			{
				NumberOfNeighbors = 6;
				CornerSize = 6;
			}
			else
			{
				CornerSize = 4;
				NumberOfNeighbors = 8;
			}

			const float HALF_CELL_WIDTH = CellWidth / 2;
			const float HALF_CELL_HEIGHT = CellHeight / 2;

			int TileIndex = 0, xTopLeft=0, yTopLeft=0;
			for (int xIterator = 0; xIterator < NumberOfColumns; xIterator++)
			{
				for (int yIterator = 0; yIterator < NumberOfRows; yIterator++)
				{
					CommonCellData[TileIndex].Column = xIterator;
					CommonCellData[TileIndex].Row = yIterator;

					// ---- init edge
					{
						if ((xIterator == 0) && (yIterator == 0))
						{
							CommonCellData[TileIndex].Edge = TOPLEFTCORNER_EDGE;
						}
						else if ((xIterator == 0) && (yIterator == NumberOfRows-1))
						{
							CommonCellData[TileIndex].Edge = BOTTOMLEFTCORNER_EDGE;
						}
						else if ((xIterator == NumberOfColumns-1) && (yIterator==0))
						{
							CommonCellData[TileIndex].Edge = TOPRIGHTCORNER_EDGE;
						}
						else if ((xIterator == NumberOfColumns-1) && (yIterator==NumberOfRows-1))
						{
							CommonCellData[TileIndex].Edge = BOTTOMRIGHTCORNER_EDGE;
						}
						else if (xIterator == 0)
						{
							CommonCellData[TileIndex].Edge = LEFT_EDGE;
						}
						else if (xIterator == NumberOfColumns-1)
						{
							CommonCellData[TileIndex].Edge = RIGHT_EDGE;
						}
						else if (yIterator == 0)
						{
							CommonCellData[TileIndex].Edge = TOP_EDGE;
						}
						else if (yIterator == NumberOfRows-1)
						{
							CommonCellData[TileIndex].Edge = BOTTOM_EDGE;
						}
						else
						{
							CommonCellData[TileIndex].Edge = NO_EDGE;
						}
					}

					CommonCellData[TileIndex].Height = CellHeight;
					CommonCellData[TileIndex].Width = CellWidth;

					// todo -- convert to triangles
					if (GridType == GRID_TYPE_ISOMETRIC)
					{
						CommonCellData[TileIndex].Corners.resize(4);

						sf::Vector2f Left, Right, Back, Front;
						//if we're in the first row we need to start things off
						if (TileIndex == 0)
						{
							Left.x = 0;
							Left.y = 0;
							Back.x = Left.x + HALF_CELL_WIDTH;
							Back.y = Left.y - HALF_CELL_HEIGHT;
							Right.x = Left.x + CellWidth;
							Right.y  = Left.y;
							Front.x = Left.x + HALF_CELL_WIDTH;
							Front.y = Back.y + CellHeight;
						}
						//if we're in the first row, place tile from front point of above left tile.
						else if (CommonCellData[TileIndex].Row == 0)
						{
							int i = getCellIndex(CommonCellData[TileIndex].Column-1, CommonCellData[TileIndex].Row);
							Left = CommonCellData[i].Corners[BACK];
							Front = CommonCellData[i].Corners[RIGHT];

							Back.x = Front.x;
							Back.y = Front.y - CellHeight;

							Right.x = Left.x + CellWidth;
							Right.y  = Left.y;
						}
						else // else, place from the top point of tile to bottomleft
						{
							int __CellIndex = getCellIndex(CommonCellData[TileIndex].Column, CommonCellData[TileIndex].Row-1);
							Left = CommonCellData[__CellIndex].Corners[FRONT];
							Back = CommonCellData[__CellIndex].Corners[RIGHT];
							Right.x = Left.x + CellWidth;
							Right.y  = Left.y;
							Front.x = Left.x + HALF_CELL_WIDTH;
							Front.y = Back.y + CellHeight;
						}

						CommonCellData[TileIndex].Corners[LEFT] = Left;
						CommonCellData[TileIndex].Corners[BACK] = Back;
						CommonCellData[TileIndex].Corners[RIGHT] = Right;
						CommonCellData[TileIndex].Corners[FRONT] = Front;
					}
					else if (GridType == GRID_TYPE_SQUARE)
					{
						CommonCellData[TileIndex].Corners.resize(4);
						CommonCellData[TileIndex].Corners[LEFT] = sf::Vector2f(xTopLeft, yTopLeft);
						CommonCellData[TileIndex].Corners[BACK] = sf::Vector2f(xTopLeft+CellWidth, yTopLeft);
						CommonCellData[TileIndex].Corners[RIGHT] = sf::Vector2f(xTopLeft+CellWidth, yTopLeft+CellHeight);
						CommonCellData[TileIndex].Corners[FRONT] = sf::Vector2f(xTopLeft, yTopLeft+CellHeight);
					}
					else // hex
					{
						CommonCellData[TileIndex].Corners.resize(6);
						CommonCellData[TileIndex].Corners[0] = sf::Vector2f(xTopLeft+ CONST_NUMBER, yTopLeft);
						CommonCellData[TileIndex].Corners[1] = sf::Vector2f(xTopLeft+CellWidth- CONST_NUMBER, yTopLeft);
						CommonCellData[TileIndex].Corners[2] = sf::Vector2f(xTopLeft+CellWidth, yTopLeft+HALF_CELL_HEIGHT);
						CommonCellData[TileIndex].Corners[3] = sf::Vector2f(xTopLeft+CellWidth- CONST_NUMBER, yTopLeft+CellHeight);
						CommonCellData[TileIndex].Corners[4] = sf::Vector2f(xTopLeft+ CONST_NUMBER, yTopLeft+CellHeight);
						CommonCellData[TileIndex].Corners[5] = sf::Vector2f(xTopLeft, yTopLeft+HALF_CELL_HEIGHT);
					}

					CommonCellData[TileIndex].Center = sf::Vector2f(xTopLeft + HALF_CELL_WIDTH, yTopLeft + HALF_CELL_HEIGHT);

					yTopLeft += CellHeight;
					TileIndex++;
				}
				xTopLeft += CellWidth;
				yTopLeft = 0;

				if (GridType == GRID_TYPE_HEX)
				{
					xTopLeft -= (CellWidth - CONST_NUMBER);
					if (xIterator % 2 == 0)
					{
						yTopLeft += HALF_CELL_HEIGHT;
					}
				}
			}


			if (GridType == GRID_TYPE_HEX)
			{
				_calculateNeighbors_HexGrid();
			}
			else
			{
				_calculateNeighbors_SquareGrid();
			}

			_addCellSpacing();
		}

	////////////////////////////////////////////////////////////

		void _addCellSpacing()
		{
			int i = 0;
			for (int x = 0; x < NumberOfColumns; x++)
			{
				for (int y = 0; y < NumberOfRows; y++)
				{
					int xSpacing = CellSpacing * x;
					int ySpacing = CellSpacing * y;

					for (int j = 0; j < CommonCellData[i].Corners.size(); j++)
					{
						CommonCellData[i].Corners[j].x += xSpacing;
						CommonCellData[i].Corners[j].y += ySpacing;
					}
					i++;
				}
			}
		}

	////////////////////////////////////////////////////////////

		void _calculateNeighbors_SquareGrid()
		{
			int Col=-1, Row=-1, NeighborIndex=-1;

			Neighbors.clear();
			Neighbors.resize((unsigned long)NumberOfCells);
			for (int CellIndex = 0; CellIndex < NumberOfCells; CellIndex++)
			{
				Col = CommonCellData[CellIndex].Column;
				Row = CommonCellData[CellIndex].Row;

				// ---- TileToLeft
				{
					int ColToTest = Col-1;
					if ((ColToTest < 0) && (HorizontalWrap))
					{
						ColToTest = NumberOfColumns-1;
					}

					NeighborIndex = getCellIndex(ColToTest, Row);
					if (isValidCell(NeighborIndex))
					{
						if ((CommonCellData[CellIndex].Column != 0) || (HorizontalWrap))
						{
							Neighbors[CellIndex].push_back(NeighborIndex);
						}
					}
				}

				// ---- TileToTopLeft
				{
					int ColToTest = Col-1;
					if ((ColToTest < 0) && (HorizontalWrap))
					{
						ColToTest = NumberOfColumns-1;
					}

					NeighborIndex = getCellIndex(ColToTest, Row-1);
					if (isValidCell(NeighborIndex))
					{
						if  (CommonCellData[CellIndex].Row != 0)
						{
							if ((CommonCellData[CellIndex].Column != 0) || (HorizontalWrap))
							{
								Neighbors[CellIndex].push_back(NeighborIndex);
							}
						}
					}
				}

				// ---- TileToTop
				{
					NeighborIndex = getCellIndex(Col, Row-1);
					if (isValidCell(NeighborIndex))
					{
						if (CommonCellData[CellIndex].Row != 0)
						{
							Neighbors[CellIndex].push_back(NeighborIndex);
						}
					}
				}

				// ---- TileToTopRight
				{
					int ColToTest = Col+1;
					if ((ColToTest >= NumberOfColumns) && (HorizontalWrap))
					{
						ColToTest = 0;
					}

					NeighborIndex = getCellIndex(ColToTest, Row-1);
					if (isValidCell(NeighborIndex))
					{
						if (CommonCellData[CellIndex].Row != 0)
						{
							if ((CommonCellData[CellIndex].Column != NumberOfColumns-1) || (HorizontalWrap))
							{
								Neighbors[CellIndex].push_back(NeighborIndex);
							}
						}
					}
				}

				// ---- TileToRight
				{
					int ColToTest = Col+1;
					if ((ColToTest >= NumberOfColumns) && (HorizontalWrap))
					{
						ColToTest = 0;
					}

					NeighborIndex = getCellIndex(ColToTest, Row);
					if (isValidCell(NeighborIndex))
					{
						if ((CommonCellData[CellIndex].Column != NumberOfColumns-1) || (HorizontalWrap))
						{
							Neighbors[CellIndex].push_back(NeighborIndex);
						}
					}
				}

				// ---- TileToBottomRight
				{
					int ColToTest = Col+1;
					if ((ColToTest >= NumberOfColumns) && (HorizontalWrap))
					{
						ColToTest = 0;
					}

					NeighborIndex = getCellIndex(ColToTest, Row+1);
					if (isValidCell(NeighborIndex))
					{
						if (CommonCellData[CellIndex].Row != NumberOfRows-1)
						{
							if ((CommonCellData[CellIndex].Column != NumberOfColumns-1) || (HorizontalWrap))
							{
								Neighbors[CellIndex].push_back(NeighborIndex);
							}
						}
					}
				}

				// ---- TileToBottom
				{
					NeighborIndex = getCellIndex(Col, Row+1);
					if (isValidCell(NeighborIndex))
					{
						if (CommonCellData[CellIndex].Row != NumberOfRows-1)
						{
							Neighbors[CellIndex].push_back(NeighborIndex);
						}
					}
				}

				// ---- TileToBottomLeft
				{
					int ColToTest = Col-1;
					if ((ColToTest < 0) && (HorizontalWrap))
					{
						ColToTest = NumberOfColumns-1;
					}

					NeighborIndex = getCellIndex(ColToTest, Row+1);
					if (isValidCell(NeighborIndex))
					{
						if (CommonCellData[CellIndex].Row != NumberOfRows-1)
						{
							if ((CommonCellData[CellIndex].Column != 0) || (HorizontalWrap))
							{
								Neighbors[CellIndex].push_back(NeighborIndex);
							}
						}
					}
				}
			}
		}

	////////////////////////////////////////////////////////////

		void _calculateNeighbors_HexGrid()
		{
			int Col=-1, Row=-1, NeighborIndex=-1;

			Neighbors.clear();
			Neighbors.resize((unsigned long)NumberOfCells);
			for (int CellIndex = 0; CellIndex < NumberOfCells; CellIndex++)
			{
				Col = CommonCellData[CellIndex].Column;
				Row = CommonCellData[CellIndex].Row;

				// ---- TileToLeft
				{
					NeighborIndex = getCellIndex(Col-1, Row);
					if (isValidCell(NeighborIndex))
					{
						if (CommonCellData[CellIndex].Column != 0)
						{
							Neighbors[CellIndex].push_back(NeighborIndex);
						}
					}
				}

				// ---- TileToTop
				{
					NeighborIndex = getCellIndex(Col-1, Row-1);
					if (isValidCell(NeighborIndex))
					{
						if ((CommonCellData[CellIndex].Column != 0) && (CommonCellData[CellIndex].Row != 0))
						{
							Neighbors[CellIndex].push_back(NeighborIndex);
						}
					}
				}

				// ---- TileToRight
				{
					NeighborIndex = getCellIndex(Col+1, Row-1);
					if (isValidCell(NeighborIndex))
					{
						if ((CommonCellData[CellIndex].Column != NumberOfColumns-1) && (CommonCellData[CellIndex].Row != 0))
						{
							Neighbors[CellIndex].push_back(NeighborIndex);
						}
					}
				}

				// ---- TileToBottomRight
				{
					NeighborIndex = getCellIndex(Col+1, Row+1);
					if (isValidCell(NeighborIndex))
					{
						if ((CommonCellData[CellIndex].Column != NumberOfColumns-1) && (CommonCellData[CellIndex].Row != NumberOfRows-1))
						{
							Neighbors[CellIndex].push_back(NeighborIndex);
						}
					}
				}

				// ---- TileToBottom
				{
					NeighborIndex = getCellIndex(Col, Row+1);
					if (isValidCell(NeighborIndex))
					{
						if (CommonCellData[CellIndex].Row != NumberOfRows-1)
						{
							Neighbors[CellIndex].push_back(NeighborIndex);
						}
					}
				}

				// ---- TileToBottomLeft
				{
					NeighborIndex = getCellIndex(Col-1, Row+1);
					if (isValidCell(NeighborIndex))
					{
						if ((CommonCellData[CellIndex].Column != 0) && (CommonCellData[CellIndex].Row != NumberOfRows-1))
						{
							Neighbors[CellIndex].push_back(NeighborIndex);
						}
					}
				}
			}
		}
	};

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

	template <class T>
	class Grid_1D_Custom_Data : public Grid_Base
	{
	public:
		// app specific data.
		std::vector<T> CustomCellData;

        void init(int NumberOfRows, int NumberOfColumns, int GridType, int CellSpacing, int CellWidth, int CellHeight, bool HorizontalWrap) override
        {
            CustomCellData.clear();
            Grid_Base::init(NumberOfRows, NumberOfColumns, GridType, CellSpacing, CellWidth, CellHeight, HorizontalWrap);
        }

        void resizeCustomCellData(int NumberOfCells)
		{
			CustomCellData.resize(NumberOfCells);
		}
		
		
		template <class Archive>
		void serialize( Archive & ar )
		{
			ar(CommonCellData);
			ar(CustomCellData);
			ar(CornerSize);
			ar(Neighbors);
			ar(NumberOfColumns);
			ar(NumberOfRows);
			ar(NumberOfCells);
			ar(HalfColumnCount);
			ar(NumberOfNeighbors);
			ar(CellSpacing);
			ar(LastColumn);
			ar(LastRow);
			ar(GridType);
			ar(TileHeight);
			ar(TileWidth);
			ar(HorizontalWrap);
		}
	};
}

#endif
