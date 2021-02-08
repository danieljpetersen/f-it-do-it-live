#include "user_interface.h"
#include "globals.h"
#include "cheats.h"
#include "simulation.h"
#include "drawable_map.h"
#include "mouse_tile_tracker.h"
#include "map.h"
#include "color_schemes.h"
#include "nations.h"
#include "vision.h"

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Plugin_User_Interface::onEnableChange(bool Enabling)
{
	if (Enabling)
	{
		reset();
	}
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Plugin_User_Interface::reset()
{
	SelectionManager.reset();
	TickController.reset();
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Plugin_User_Interface::work(const int Event)
{
	switch (Event)
	{
		case fi::EVENT_UPDATE:
		{
			onUpdate();
			break;
		}

		case fi::EVENT_DRAW:
		{
			onDraw();
			break;
		}

		case EVENT_MAP_GENERATION:
		{
			reset();
			break;
		}

		default:
		{
			fi::log("plugin user interface called with event " + fi::to_s(Event) + " for which it has no handler");
			break;
		}
	}
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Plugin_User_Interface::onUpdate()
{
	if (getCoreTick()->isStepThisFrame())
	{
		for (auto Path : Paths)
		{
			getSimulation().NextSimulationInput->Paths.push_back(Path.second);
		}

		Paths.clear();
	}

	guiCore();

	if (fi::getGUI().Handled)
	{
		return;
	}

	bool WasUnitSelected = SelectionManager.isUnitSelected();
	bool IsUnitSelected = WasUnitSelected;

	SelectionManager.onUpdate();

	if (SelectionManager.isUnitSelected())
	{
		IsUnitSelected = true;
		if (WasUnitSelected)
		{
			if (fi::getInput().check(fi::key::Mouse1))
			{
				int GoalTile = -1;
				for (auto SelectedID : SelectionManager.Selected)
				{
					auto Unit = getUnits().getReadPtr()->get(SelectedID);

					if (GoalTile == -1)
					{
						GoalTile = getMouseTileTracker().MouseTileIndex;

						// TODO THIS IS WRONG
						// 		units on different continents should move to that continent across ocean.
						// 		if not ocean capable it should move to the closest tile on its continent. meaning we need to perform the finndclosest search once per continent which containns a unit

						// MouseTileIndex isn't necessarily the desired GoalTile. If not water capable or clicked on coastal tile, find closest tile on same continent
						if (getGrid().CustomCellData[Unit->TileIndex].ContinentIndex != getGrid().CustomCellData[GoalTile].ContinentIndex)
						{
							int TileIndex = Unit->TileIndex;
							int ContinentIndex = getGrid().CustomCellData[TileIndex].ContinentIndex;

							if (ContinentIndex != -1)
							{
								int ContinentLocal = getTiles().findClosestTileFromTileIndex_returnsLocalIndex(getTiles().Continents[ContinentIndex], getMouseTileTracker().MouseTileIndex, false);
								GoalTile = getTiles().Continents[ContinentIndex][ContinentLocal];
							}
						}
					}

					if (getCoreTick()->isStepThisFrame())
					{
						// pita - make it nqueued path
						//Paths[Unit->getSlotMapID()] = getPathfinding().findPath(Unit->, GoalTile, false);
					}
					else
					{
						Paths[Unit->getSlotMapID()] = getPathfinding().findPath(Unit, GoalTile, false);
					}
				}
			}
		}
	}
	else
	{
		IsUnitSelected = false;
	}

	if (fi::getInput().check(fi::key::U))
	{
		getMapDrawables().ShowDebugProvinceTiles = !getMapDrawables().ShowDebugProvinceTiles;
		fi::getPlugins().delayedExecute(EVENT_BUILD_MAP_DRAWABLES);
	}

	if (fi::getInput().check(fi::key::Mouse1, fi::KEYSTATE_RELEASED))
	{
		if (IsUnitSelected != true)
		{
			// for debug
			//getSimulation().NextSimulationInput->UnitCreationDestructionRequests.unitCreationRequest(getNations().HumanNationIndex, getMouseTileTracker().MouseTileIndex, 0, false);
		}
	}

	// ---- load / save / map gen
	{
		if (fi::getInput().check("Generate New Map"))
		{
			fi::getPlugins().delayedExecute(EVENT_MAP_GENERATION, fi::EVENT_DRAW);
		}
		//else if (fi::getInput().check(fi::key::code::L))
		//{
		//    fi::getEngine().triggerLoadData(getApp().ProgramState_Game);
		//}
		//else if (fi::getInput().check(fi::key::code::K))
		//{
		//    fi::Engine::instance().executeSaveData();
		//}
	}

	// ---- cheats
	{
		if (fi::getInput().check("Cheat Reveal Terrain"))
		{
			getCheats().toggleRevealTerrain();
		}

		if (fi::getInput().check("Cheat Reveal Fog of War"))
		{
			getCheats().toggleFow();
		}
	}

	fi::getCanvasWorld().handleCameraInput(true, true, true, true, true);
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Plugin_User_Interface::onDraw()
{
	SelectionManager.onDraw();

	sf::VertexArray Lines(sf::Lines);
	auto UnitsReadPtr = getUnits().getReadPtr();
	for (auto SelectedID : SelectionManager.Selected)
	{
		IUEINW_Path Path;

		bool ok = false;
		auto iter = Paths.find(SelectedID);
		if (iter != Paths.end())
		{
			ok = true;
			Path = iter->second;
		}
		else
		{
			auto Unit = UnitsReadPtr->get(SelectedID);

			if (Unit != nullptr)
			{
				if (Unit->Path.Path != nullptr)
				{
					ok = true;
					Path = Unit->Path;
				}
			}
		}

		if (ok)
		{
			if (Path.Path->empty() != true)
			{
				int PreviousIndex = Path.Path->at(0);
				for (int i = 1; i < Path.Path->size(); i++)
				{
					int TileIndex = Path.Path->at(i);

					sf::Vector2f a = getGrid().CommonCellData[PreviousIndex].Center;
					sf::Vector2f b = getGrid().CommonCellData[TileIndex].Center;
					fi::Draw::line(Lines, a, b, getColorSchemes().getSelectionColor());

					PreviousIndex = TileIndex;
				}
			}
		}
	}

	fi::getCanvasWorld().draw(Lines);
}


////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Plugin_User_Interface::guiCore()
{
    // ---- menubar
    {
        fi::getGUI().selectNode(0);
        int Menu = fi::guiPanel(new fi::GUI_Node(fi::ANCHOR_START_ABSOLUTE, fi::ANCHOR_START,
                                                 fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_X, 1.0f),
                                                 fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_Y, fi::rowHeight(), 0.0f)));

        fi::getGUI().selectNode(Menu);

        TickController.update(getCoreTick());

        // ----
        int ButtonSize = 77;
        int HandledIndex = -1;
        std::vector<std::string> Labels = { "Game", "View", "Cheats" };
        if (fi::guiButtonsHorizontal(new fi::GUI_Node(fi::ANCHOR_START, fi::ANCHOR_START_ABSOLUTE, sf::Vector2f(ButtonSize, fi::rowHeight())), Labels, HandledIndex))
        {

        }
    }
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Unit_Selection_Manager::performUnitMoveCommand()
{
	fi::log("move");
}

////////////////////////////////////////////////////////////

std::vector<fi::Slot_Map_ID> IUEINW::IUEINW_Unit_Selection_Manager::getUnitsInRect(sf::Vector2f SelectionTopLeft, int SelectionWidth, int SelectionHeight)
{
	std::vector<fi::Slot_Map_ID> UnitsInRect;

	auto UnitsReadPtr = getUnits().Units.ReadPtr;
	for (int i = 0; i < UnitsReadPtr->size(); i++)
	{
		auto Position = UnitsReadPtr->Objects[i].getTileCenterPosition();

		if (fi::Math::isPointInBounds(UnitsReadPtr->Objects[i].getTileCenterPosition(), SelectionTopLeft, SelectionWidth, SelectionHeight))
		{
			UnitsInRect.push_back(UnitsReadPtr->Objects[i].getSlotMapID());
		}

	}

	return UnitsInRect;
}

////////////////////////////////////////////////////////////

fi::Slot_Map_ID IUEINW::IUEINW_Unit_Selection_Manager::getUnitClosestToMouse()
{
	auto BestId = fi::Slot_Map_ID::null();
	float BestDistance = std::numeric_limits<float>::max();
	auto UnitsReadPtr = getUnits().Units.ReadPtr;
	for (int i = 0; i < UnitsReadPtr->size(); i++)
	{
		float Distance = fi::Math::distance(fi::getMouseWorldPosition2f(), UnitsReadPtr->Objects[i].getTileCenterPosition());
		if (Distance < BestDistance)
		{
			BestDistance = Distance;
			BestId = UnitsReadPtr->Objects[i].getSlotMapID();
		}
	}

	return BestId;
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Unit_Selection_Manager::onSelectionChange(bool UnitsDeselected, bool UnitsSelected)
{
	if (UnitsDeselected)
	{
		fi::log("on deselected todo");
	}

	if (UnitsSelected)
	{
		fi::log("on selected todo");
	}
}

////////////////////////////////////////////////////////////

std::set<int> IUEINW::IUEINW_Unit_Selection_Manager::getSelectedTiles()
{
	std::set<int> SelectedTiles;

	auto UnitsReadPtr = getUnits().getReadPtr();

	for (auto SelectedUnitID : Selected)
	{
		auto Unit = UnitsReadPtr->get(SelectedUnitID);

		SelectedTiles.insert(Unit->TileIndex);
	}

	return std::move(SelectedTiles);
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Unit_Selection_Manager::onDraw()
{
	fi::Selection_Manager::onDraw();

	auto SelectedTiles = getSelectedTiles();

	auto Grid = &getGrid();

	sf::VertexArray VertexArray(sf::Lines);

	// todo apply column transform

	for (auto TileIndex : SelectedTiles)
	{
		VertexArray.append(sf::Vertex(Grid->CommonCellData[TileIndex].getLeft2f(), getColorSchemes().getSelectionColor()));
		VertexArray.append(sf::Vertex(Grid->CommonCellData[TileIndex].getBack2f(), getColorSchemes().getSelectionColor()));

		VertexArray.append(sf::Vertex(Grid->CommonCellData[TileIndex].getRight2f(), getColorSchemes().getSelectionColor()));
		VertexArray.append(sf::Vertex(Grid->CommonCellData[TileIndex].getFront2f(), getColorSchemes().getSelectionColor()));

		VertexArray.append(sf::Vertex(Grid->CommonCellData[TileIndex].getLeft2f(), getColorSchemes().getSelectionColor()));
		VertexArray.append(sf::Vertex(Grid->CommonCellData[TileIndex].getFront2f(), getColorSchemes().getSelectionColor()));

		VertexArray.append(sf::Vertex(Grid->CommonCellData[TileIndex].getBack2f(), getColorSchemes().getSelectionColor()));
		VertexArray.append(sf::Vertex(Grid->CommonCellData[TileIndex].getRight2f(), getColorSchemes().getSelectionColor()));
	}

	fi::getCanvasWorld().draw(VertexArray);
}
