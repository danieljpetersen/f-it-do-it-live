#include "selection_manager.h"
#include "input.h"
#include "engine.h"

////////////////////////////////////////////////////////////

void fi::Selection_Manager::reset()
{
    ControlGroups.clear();
    Selected.clear();
}

////////////////////////////////////////////////////////////

void fi::Selection_Manager::onUpdate()
{
    bool QueueKeyHeldDown = false;
    if (fi::getInput().check("Queue Key"))
    {
        QueueKeyHeldDown = true;
    }

    UnitsSelectedThisFrame = false;
    UnitsDeselectedThisFrame = false;

    // ----

    bool ControlGroupModifier = fi::getInput().check("Control Group Modifier");

    std::vector<int> NumberKeys = fi::getInput().NumberKeysPressed;
    for (unsigned int i = 0; i < NumberKeys.size(); i++)
    {
    	if (NumberKeys[i] == 0)
    		continue;

    	NumberKeys[i] -= 1;
    	if (ControlGroupModifier)
    	{
    		unitsSelectedToControlGroup_OverwriteList(NumberKeys[i]);
    	}
    	else if (QueueKeyHeldDown)
    	{
    		unitsSelectedToControlGroup_AppendToList(NumberKeys[i]);
    	}
    	else
    	{
    		controlGroupToUnitsSelected(NumberKeys[i]);
    	}
    }

    // ----

    if (fi::getInput().check("Deselect Units"))
    {
        deselectUnits();
    }

    if (fi::getInput().check("Select Previous Unit"))
    {
        selectPreviousUnit();
    }

    if (fi::getInput().check("Select Next Unit"))
    {
        selectNextUnit();
    }


    // ---- double click
    if (fi::getGUI().MouseWithinBounds != true)
    {
        if (fi::getInput().check(fi::key::Mouse1, fi::KEYSTATE_DOUBLE_TAPPED))
        {
            if (Selected.size())
            {
                if (true) // todo mouse distance
                {
                    UnitSelectedOrDeselected = true;
                    deselectUnits();
                }
            }
            else
            {
                UnitSelectedOrDeselected = true;

                // if there are no units within the selection rect then we want to select the closest unit to the current position of the mouse
                auto UnitIDClosestToMouse = getUnitClosestToMouse();
                if (UnitIDClosestToMouse != fi::Slot_Map_ID::null())
				{
                	selectUnit(UnitIDClosestToMouse);
				}
            }
        }

            //else if (fi::getInput().check("Unit Controller Initial Button Down Action"))
        if (fi::getInput().check(fi::key::Mouse1, fi::KEYSTATE_INITIAL_DOWN))
        {
            if (Selected.size() == 0)
            {
                BoxSelectInitialMouseDownPosition = fi::getInput().MouseWorldPosition;
            }
        }

        //else if (fi::getInput().check("Unit Controller Button Released Action"))
        if (fi::getInput().check(fi::key::Mouse1, fi::KEYSTATE_RELEASED))
        {
            if (UnitSelectedOrDeselected != true)
            {
                onActionButtonReleased(QueueKeyHeldDown);
            }

            BoxSelectInitialMouseDownPosition.x = -1;
            BoxSelectInitialMouseDownPosition.y = -1;
            UnitSelectedOrDeselected = false;
        }
    }

    onSelectionChange(UnitsDeselectedThisFrame, UnitsSelectedThisFrame);
}

////////////////////////////////////////////////////////////

void fi::Selection_Manager::onDraw()
{
    if (isSelectionBoxActive())
    {
        sf::Vector2i SelectionTopLeft;
        int SelectionWidth = -1, SelectionHeight = -1;
        fi::Math::getRect(BoxSelectInitialMouseDownPosition, fi::getMouseWorldPosition2i(), SelectionTopLeft, SelectionWidth, SelectionHeight);
        fi::Draw::rectOutline(fi::getCanvasWorld().getRenderTarget(), SelectionTopLeft, SelectionWidth, SelectionHeight, sf::Color::White);
    }
}

////////////////////////////////////////////////////////////

void fi::Selection_Manager::deselectUnit(fi::Slot_Map_ID ID)
{
    // Iterate through all the elements in a set and display the value.
    for (std::set<fi::Slot_Map_ID>::iterator it=Selected.begin(); it!=Selected.end(); ++it)
    {
        if ((*it).getSlotMapID() == ID)
        {
            Selected.erase(it);
            UnitsDeselectedThisFrame = true;
            break;
        }
    }
}

////////////////////////////////////////////////////////////

void fi::Selection_Manager::deselectUnits()
{
    Selected.clear();
    UnitsDeselectedThisFrame = true;
}

////////////////////////////////////////////////////////////

void fi::Selection_Manager::selectUnit(fi::Slot_Map_ID ID)
{
    Selected.insert(ID);
    UnitsSelectedThisFrame = true;
}

////////////////////////////////////////////////////////////

void fi::Selection_Manager::selectUnits(std::vector<fi::Slot_Map_ID> Units)
{
    deselectUnits();

    for (unsigned int i = 0; i < Units.size(); i++)
    {
        selectUnit(Units[i]);
    }
}

////////////////////////////////////////////////////////////

void fi::Selection_Manager::selectNextUnit()
{
    // todo
    fi::log("todo select next unit");
}

////////////////////////////////////////////////////////////

void fi::Selection_Manager::selectPreviousUnit()
{
    // todo
    fi::log("todo select prev unit");
}

////////////////////////////////////////////////////////////

bool fi::Selection_Manager::isUnitSelected()
{
	return ! Selected.empty();
}

////////////////////////////////////////////////////////////

bool fi::Selection_Manager::isUnitSelected(fi::Slot_Map_ID ID)
{
    for (std::set<fi::Slot_Map_ID>::iterator it=Selected.begin(); it!=Selected.end(); ++it) // todo --- performance...
    {
        if ((*it).getSlotMapID() == ID)
        {
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////

void fi::Selection_Manager::controlGroupToUnitsSelected(int ControlGroup)
{
    std::set<int> NewSelection;
    selectUnits(ControlGroups[ControlGroup]);
}

////////////////////////////////////////////////////////////

void fi::Selection_Manager::unitsSelectedToControlGroup_OverwriteList(int ControlGroup)
{
    ControlGroups[ControlGroup].clear();
    for (auto ID : Selected)
    {
    	ControlGroups[ControlGroup].push_back(ID);
    }
}

////////////////////////////////////////////////////////////

void fi::Selection_Manager::unitsSelectedToControlGroup_AppendToList(int ControlGroup)
{
    std::set<fi::Slot_Map_ID> UnitsOriginallyPresentInControlGroup;
    for (unsigned int i = 0; i < ControlGroups[ControlGroup].size(); i++)
    {
    	UnitsOriginallyPresentInControlGroup.insert(ControlGroups[ControlGroup][i]);
    }

    for (auto ID : Selected)
    {
    	if (UnitsOriginallyPresentInControlGroup.count(ID) == 0)
    	{
    		ControlGroups[ControlGroup].push_back(ID);
    	}
    }
}

////////////////////////////////////////////////////////////

void fi::Selection_Manager::removeFromControlGroups(fi::Slot_Map_ID UnitsArrayIndex)
{
    for (unsigned int i = 0; i < ControlGroups.size(); i++)
    {
        ControlGroups[i].erase(std::remove(ControlGroups[i].begin(), ControlGroups[i].end(), UnitsArrayIndex), ControlGroups[i].end());
    }
}

////////////////////////////////////////////////////////////

void fi::Selection_Manager::resetSelectionBox()
{
    BoxSelectInitialMouseDownPosition = SELECTION_BOX_INACTIVE_COORDS;
}

////////////////////////////////////////////////////////////

bool fi::Selection_Manager::isSelectionBoxActive()
{
    return (BoxSelectInitialMouseDownPosition.x != -1) && (BoxSelectInitialMouseDownPosition.y != -1);
}

////////////////////////////////////////////////////////////

bool fi::Selection_Manager::onActionButtonReleased(bool AddToQueue)
{
    bool Handled = false;

    if (Selected.size())
    {
        performUnitMoveCommand();
    }
    else
    {
        if (isSelectionBoxActive())
        {
            sf::Vector2i SelectionTopLeft(-1, -1);
            int SelectionWidth = -1, SelectionHeight = -1;
            fi::Math::getRect(BoxSelectInitialMouseDownPosition, fi::getMouseWorldPosition2i(), SelectionTopLeft, SelectionWidth, SelectionHeight);

            std::vector<fi::Slot_Map_ID> UnitsWithinSelectionRect = getUnitsInRect(fi::convert(SelectionTopLeft), SelectionWidth, SelectionHeight);

            if (UnitsWithinSelectionRect.size())
            {
                selectUnits(UnitsWithinSelectionRect);
                Handled = true;
            }
        }
    }

    return Handled;
}

////////////////////////////////////////////////////////////
