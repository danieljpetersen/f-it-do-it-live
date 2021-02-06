#include "../common/engine.h"

/*
 	eventual goal is to produce something akin to https://github.com/fogleman/primitive but do so quickly for the purpose of transforming videos into primitive videos

    Roughly how it works:
    Each image in the "images_for_conversion" folder will be recreated using only triangles, possibly other shapes later down the line
    It does this via partitioning the image into various sized grids. 
    There might be a grid which has cells 5 pixels each, a grid with cells 100 pixels each, etc.
    Each cell will be assigned a color which is determined by averaging the color of each pixel in that specific cell from the source image
    The triangles are then built by running over these grids. Each cell for each color grid has the possibility of spawning a triangle.
    The triangle gets one randomly placed vertex inside that specific cell, and it's two corresponding vertexes can appear in neighboring cells.
    Neighboring cells are only valid for placing a vertex if their averaged color is close enough to the starting cell by some threshold.
    Cells aren't limited to their immediate neighbor. It spreads out from it's starting cell, stopping either randomly or stopping if the color threshold is too far

    alternative idea worth trying:
    	you can get the median color for all squares in a given triangle (can be square approximated from triangle if easier)
		this would be color of triangle for randomly placed vertices (constrained by some max length for triangle size)
*/

// todo this doesn't quite work as it should because fi::Grid isn't laid out the same as sf::image, the indexes are off. grid goes column by column because of world wrap in iueinw, image goes row by row

namespace TGV
{
    const int WORK_ORDER_TYPE_BUILD_COLOR_GRID = 0;
    const int WORK_ORDER_TYPE_BUILD_VERTEX_ARRAY = 1;

    class Work_Order
    {
    public:
        int WorkOrderType;

        int Alpha;
        int CellSize;
        sf::Image *Image;
        std::shared_ptr<sf::VertexArray> VertexArray;
        std::shared_ptr<fi::Grid_1D_Custom_Data<sf::Color>> Grid;
        std::vector<std::shared_ptr<fi::Grid_1D_Custom_Data<sf::Color>>> *AllColorGrids;
    };

    // ----

    static atomicwrapper<int> WorkCompletionCount(0);
    static atomicwrapper<int> WorkJobCount(0);
    static atomicwrapper<bool> BackgroundThreadQuit(false);
    static moodycamel::BlockingConcurrentQueue<Work_Order> WorkQueue;
    const sf::Vector2f OutputResolution = sf::Vector2f(1920, 1080);

    // ----

    void asyncWork(Work_Order WorkOrder)
	{
        int CellSize = WorkOrder.CellSize;
        sf::Image *Image = WorkOrder.Image;

        int ImageWidth = Image->getSize().x;
        int ImageHeight = Image->getSize().y;

        const int NumberOfRows = (int)std::floor(ImageHeight / CellSize);
        const int NumberOfCols = (int)std::floor(ImageWidth / CellSize);

        if (WorkOrder.WorkOrderType == WORK_ORDER_TYPE_BUILD_COLOR_GRID)
        {
            WorkOrder.Grid->init(NumberOfRows, NumberOfCols, fi::GRID_TYPE_SQUARE, 0, CellSize, CellSize, false);

            for (int ColumnCounter = 0; ColumnCounter < NumberOfCols; ColumnCounter++)
            {
                for (int RowCounter = 0; RowCounter < NumberOfRows; RowCounter++)
                {
                    int x = ColumnCounter * CellSize;
                    int y = RowCounter * CellSize;

                    int r = 0;
                    int g = 0;
                    int b = 0;
                    int PixelCount = 0;
                    for (int inner = 0; inner < CellSize; inner++)
                    {
                        for (int outter = 0; outter < CellSize; outter++)
                        {
                            int xPixel = x + inner;
                            int yPixel = y + outter;

                            if ((xPixel < ImageWidth) && (yPixel < ImageHeight))
                            {
                                auto Color = Image->getPixel(xPixel, yPixel);
                                r += Color.r;
                                g += Color.g;
                                b += Color.b;
                                PixelCount++;
                            }
                        }
                    }

                    if (PixelCount == 0)
                    {
                        break;
                    }

                    r /= PixelCount;
                    g /= PixelCount;
                    b /= PixelCount;

                    int CellIndex = WorkOrder.Grid->getCellIndex(ColumnCounter, RowCounter);
                    WorkOrder.Grid->CustomCellData[CellIndex] = sf::Color(r, g, b);
                }
            }
        }
        else
        {
            std::vector<bool> Processed(WorkOrder.Grid->size(), false);

            int LowestResolutionGridIndex = 0;
            for (int i = 1; i < WorkOrder.AllColorGrids->size(); i++)
            {
                if (WorkOrder.AllColorGrids->at(i)->TileWidth < WorkOrder.AllColorGrids->at(LowestResolutionGridIndex)->TileWidth )
                {
                    LowestResolutionGridIndex = i;
                }
            }

            for (int ColumnCounter = 0; ColumnCounter < NumberOfCols; ColumnCounter++)
            {
                for (int RowCounter = 0; RowCounter < NumberOfRows; RowCounter++)
                {
                    int StartingCellIndex = WorkOrder.Grid->getCellIndex(ColumnCounter, RowCounter);
                    if (Processed[StartingCellIndex] != true)
                    {
                        const int VertCount = 3;
                        //const int VertCount = 4;
                        if (VertCount == 3)
                        {
                            WorkOrder.VertexArray->setPrimitiveType(sf::Triangles);
                        }
                        else
                        {
                            WorkOrder.VertexArray->setPrimitiveType(sf::Quads);
                        }
                        std::vector<int> CellIndexes(VertCount, StartingCellIndex);

                        sf::Color StartingCellColor = WorkOrder.Grid->CustomCellData[StartingCellIndex];

                        // ---- okay -- spreads out to immediate neighbors
                        {
                            auto Neighbors = &WorkOrder.Grid->Neighbors[StartingCellIndex];
                            int _CellIndex = 1;
                            for (int i = 0; i < Neighbors->size(); i++)
                            {
                                int NeighborIndex = Neighbors->at(i);
                                sf::Color NeighborColor = WorkOrder.Grid->CustomCellData[NeighborIndex];
                                double ClosenessRank = fi::colorRankCloseness(StartingCellColor, NeighborColor);
                                if (ClosenessRank <= 200)
                                {
                                    CellIndexes[_CellIndex] = NeighborIndex;
                                    _CellIndex++;

                                    if (_CellIndex >= CellIndexes.size())
                                    {
                                        break;
                                    }
                                }
                            }
                        }


                        // working shit
                        {
                            int a = WorkOrder.Alpha;
                            for (int i = 0; i < CellIndexes.size(); i++)
                            {
                                // top bounds of the vertex spawn area
                                int x = WorkOrder.Grid->CommonCellData[CellIndexes[i]].Column * CellSize;
                                int y = WorkOrder.Grid->CommonCellData[CellIndexes[i]].Row * CellSize;
                                int VertexSpawnArea = CellSize*2; // you want some overdraw to avoid black lines along cell edges

                                sf::Vector2f VertexPosition = sf::Vector2f(fi::getRandom().i(x, x + VertexSpawnArea), fi::getRandom().i(y, y + VertexSpawnArea));
                                VertexPosition.x = std::max(0, (int)VertexPosition.x);
                                VertexPosition.x = std::min(ImageWidth-1, (int)VertexPosition.x);
                                VertexPosition.y = std::max(0, (int)VertexPosition.y);
                                VertexPosition.y = std::min(ImageHeight-1, (int)VertexPosition.y);

                                sf::Color Color = WorkOrder.Grid->CustomCellData[CellIndexes[i]];
                                Color.a = a;
                                fi::colorModify(Color, 30);

                                int col =  int(std::floor((float)Image->getSize().x / VertexPosition.x));
                                int row =  int(std::floor((float)Image->getSize().y / VertexPosition.y));
                                int Index = (Image->getSize().x * row) + col;

                                //return (NumberOfColumns * Col) + Row; // for when grid goes left to right, top to bototm
                                //return (NumberOfRows * __Col) + __Row; 	    // for when grid goes top to bottom, left to right
                                if (WorkOrder.AllColorGrids->at(LowestResolutionGridIndex)->isValidCell(Index))
                                {
                                    //int Index = CellIndexes[i];
                                    //sf::Color Color = WorkOrder.Grid->CustomCellData[CellIndexes[i]];
                                    //Color = WorkOrder.AllColorGrids->at(LowestResolutionGridIndex)->CustomCellData[Index];
                                    //Color.a = a;
                                }
                                WorkOrder.VertexArray->append(sf::Vertex(VertexPosition, Color));
                            }
                        }


                        // alt rect shit
                        //sf::Vector2f aVert;
                        //int a = WorkOrder.Alpha;
                        //
                        //// top bounds of the vertex spawn area
                        //int x1 = WorkOrder.Grid->CommonCellData[CellIndexes[0]].Column * CellSize;
                        //int y1 = WorkOrder.Grid->CommonCellData[CellIndexes[0]].Row * CellSize;
                        //
                        //int x2 = WorkOrder.Grid->CommonCellData[CellIndexes[1]].Column * CellSize;
                        //int y2 = WorkOrder.Grid->CommonCellData[CellIndexes[1]].Row * CellSize;
                        //
                        //int VertexSpawnArea = CellSize*2; // you want some overdraw to avoid black lines along cell edges
                        //x1 = fi::getRandom().i(x1, x1 + VertexSpawnArea);
                        //y1 = fi::getRandom().i(y1, y1 + VertexSpawnArea);
                        //x2 = fi::getRandom().i(x2, x2 + VertexSpawnArea);
                        //y2 = fi::getRandom().i(y2, y2 + VertexSpawnArea);
                        //
                        //x1 = std::max(0, x1);
                        //x1 = std::min(ImageWidth-1, x1);
                        //y1 = std::max(0, y1);
                        //y1 = std::min(ImageHeight-1, y1);
                        //x2 = std::max(0, x2);
                        //x2 = std::min(ImageWidth-1, x2);
                        //y2 = std::max(0, y2);
                        //y2 = std::min(ImageHeight-1, y2);
                        //
                        //sf::Color Color1 = WorkOrder.Grid->CustomCellData[CellIndexes[0]];
                        //sf::Color Color2 = WorkOrder.Grid->CustomCellData[CellIndexes[1]];
                        //
                        ////Color1 = Image->getPixel(x1, y1);
                        ////Color2 = Image->getPixel(x2, y2);
                        //
                        //Color1.a = a;
                        //Color2.a = a;
                        //
                        //sf::VertexArray DUMB;
                        //fi::Draw::line(DUMB, sf::Vector2i(x1, y1), sf::Vector2i(x2, y2), Color1, Color2, CellSize);
                        //
                        //for (int i = 0; i < DUMB.getVertexCount(); i++)
                        //{
                        //    WorkOrder.VertexArray->append(DUMB[i]);
                        //}
                    }
                }
            }
        }
	}

	// ----

	void threadLoop()
    {
        while (! BackgroundThreadQuit._a.load())
        {
            Work_Order WorkOrder;

            if (WorkQueue.wait_dequeue_timed(WorkOrder, 1))
            {
                asyncWork(WorkOrder);

                auto Value = WorkCompletionCount._a.fetch_add(1) + 1;
                if (Value >= WorkJobCount._a.load())
                {
                    BackgroundThreadQuit._a.store(true);
                }
            }
        }
    }

	// ----

    class TGV_Application : public fi::Application_Base, public fi::Plugin_Base
    {
    public:
        int ProgramState_GeometizeImage = -1;
        std::vector<std::thread> Threads;
        std::vector<std::string> ImagePaths;
        std::vector<std::shared_ptr<sf::VertexArray>> VertexArrays;
        std::vector<std::shared_ptr<fi::Grid_1D_Custom_Data<sf::Color>>> ColorGrids;
        sf::RenderTexture RenderTexture;
        int CurrentImageIndex = 0;
        sf::Image TargetImage;
        sf::Time TimeOfLastRenderDelayEvent; // controls when we render next frame. delays rendering on things like renderComplete, camera movement (because program freezes during render)

        void setup() override
        {
            fi::getPlugins().defineProgramState(ProgramState_GeometizeImage)
        	    .withPlugin(this, fi::EVENT_UPDATE);

            fi::getPlugins().setProgramState(ProgramState_GeometizeImage);

            ImagePaths = fi::getOS().getFilesInDirectory(fi::getOS().getPathToExecutableDirectory() + "images");
            sort(ImagePaths.begin(), ImagePaths.end());

            fi::getEngine().setBackgroundColor(sf::Color(42, 33, 28));
            fi::getEngine().setBackgroundColor(sf::Color(222,222,222));
            fi::getEngine().setBackgroundColor(sf::Color(0,0,0));
            buildNextFrame();
        }

        void startThreads()
        {
            Threads.clear();
            const int NumberOfBGThreads = std::max(1, (int)std::thread::hardware_concurrency() - 1);
            for (int i = 0; i < NumberOfBGThreads; i++)
            {
                Threads.push_back(std::thread(threadLoop));
            }
        }

        void joinThreads()
        {
            for (int i = 0; i < Threads.size(); i++)
            {
                Threads[i].join();
            }
            Threads.clear();
        }

        void work(const int EventType) override
        {
            if (fi::getCanvasWorld().handleCameraInput(true, true, true, true, true) != true)
            {
                if (fi::getClock().getElapsedTime().asSeconds() - TimeOfLastRenderDelayEvent.asSeconds() > 1)
                {
                    buildNextFrame();
                }
            }
            else
            {
                TimeOfLastRenderDelayEvent = fi::getClock().getElapsedTime();
            }

            // ---- draw to window
            {
                for (int i = 0; i < VertexArrays.size(); i++)
                {
                    fi::getCanvasWorld().draw(*VertexArrays[i]);
                }
            }
        }

        void buildNextFrame()
        {
            TimeOfLastRenderDelayEvent = fi::getClock().getElapsedTime();

            nextImage();

            std::vector<int> GridSizes { 14, 16, 15, 14, 16, 15 };
            std::vector<int> GridAlphas { 255, 223, 123, 113, 112, 23 };

            // ---- build color grids
            {
                WorkJobCount._a.store(0);
                WorkCompletionCount._a.store(0);
                BackgroundThreadQuit._a.store(false);

                addThreadWork_BuildColorGrids(GridSizes);

                startThreads();
                joinThreads();

            }

            // ---- build vertex arrays
            {
                WorkJobCount._a.store(0);
                WorkCompletionCount._a.store(0);
                BackgroundThreadQuit._a.store(false);

                for (int j = 0; j < 4; j++)
                {
                    for (int i = 0; i < GridSizes.size(); i++)
                    {
                        addThreadWork_BuildVertexArray(i, GridAlphas[i]);
                    }
                }

                startThreads();
                joinThreads();

                saveImageToFile();
            }
        }

        void addThreadWork_BuildColorGrids(std::vector<int> &GridSizes)
        {
            ColorGrids.clear();

            for (int i = 0; i < GridSizes.size(); i++)
            {
                WorkJobCount._a.fetch_add(1);

                ColorGrids.push_back(std::shared_ptr<fi::Grid_1D_Custom_Data<sf::Color>>(new fi::Grid_1D_Custom_Data<sf::Color>()));

                Work_Order WorkOrder;
                WorkOrder.VertexArray = nullptr; // not used for this job type
                WorkOrder.Alpha = -1; // not used for this job type
                WorkOrder.WorkOrderType = WORK_ORDER_TYPE_BUILD_COLOR_GRID;
                WorkOrder.Image = &TargetImage;
                WorkOrder.CellSize = GridSizes[i];
                WorkOrder.Grid = ColorGrids[i];
                WorkOrder.AllColorGrids = &ColorGrids;
                WorkQueue.enqueue(WorkOrder);
            }
        }

        void addThreadWork_BuildVertexArray(int GridIndex, int Alpha)
        {
            WorkJobCount._a.fetch_add(1);

            VertexArrays.push_back(std::shared_ptr<sf::VertexArray>(new sf::VertexArray(sf::PrimitiveType::Quads)));

            Work_Order WorkOrder;
            WorkOrder.WorkOrderType = WORK_ORDER_TYPE_BUILD_VERTEX_ARRAY;
            WorkOrder.VertexArray = VertexArrays.back();
            WorkOrder.Image = &TargetImage;
            WorkOrder.Grid = ColorGrids[GridIndex];
            WorkOrder.Alpha = Alpha;
            WorkOrder.CellSize = ColorGrids[GridIndex].get()->TileWidth; // width and height will be the same
            WorkOrder.AllColorGrids = &ColorGrids;
            WorkQueue.enqueue(WorkOrder);
        }

        void nextImage()
        {
            VertexArrays.clear();

            if (ImagePaths.empty() != true)
            {
            	bool Retry = false;
            	if (ImagePaths[CurrentImageIndex] != "screenshots")
				{
					TargetImage.loadFromFile(fi::getOS().getPathToExecutableDirectory() + "images/" + ImagePaths[CurrentImageIndex]);
					RenderTexture.create(TargetImage.getSize().x, TargetImage.getSize().y);
				}
            	else
				{
            		Retry = true;
				}

                CurrentImageIndex++;
                if (CurrentImageIndex >= ImagePaths.size())
                {
                    CurrentImageIndex = 0;
                }

                if (Retry)
				{
                	nextImage();
				}
            }
        }

        void saveImageToFile()
        {
            RenderTexture.clear(fi::getEngine().getBackgroundColor());
            for (int i = 0; i < VertexArrays.size(); i++)
            {
                RenderTexture.draw(*VertexArrays[i]);
            }

            RenderTexture.display();

            sf::Image Image;
            Image.create(RenderTexture.getSize().x, RenderTexture.getSize().y);
            Image.copy(RenderTexture.getTexture().copyToImage(), 0, 0);
            Image.saveToFile(fi::getOS().getPathToExecutableDirectory() + "images/screenshots/" + fi::to_s((int)fi::getEngine().getFrameCount()) + "-" + fi::datetime_to_s() + ".png");
        }
    };
}

int main()
{
    TGV::TGV_Application Application;

    fi::getEngine().run(&Application);

    return 0;
}