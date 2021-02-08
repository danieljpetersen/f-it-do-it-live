#ifndef GR_SELECTION_MANAGER_H
#define GR_SELECTION_MANAGER_H

#include "slot_map.h"
#include <set>
#include <SFML/Graphics.hpp>

////////////////////////////////////////////////////////////

namespace fi
{
    class Selection_Manager
    {
    public:
        virtual void reset();
        virtual void onUpdate();
        virtual void onDraw();

        virtual void performUnitMoveCommand() = 0;
        virtual std::vector<fi::Slot_Map_ID> getUnitsInRect(sf::Vector2f SelectionTopLeft, int SelectionWidth, int SelectionHeight) = 0;
        virtual fi::Slot_Map_ID getUnitClosestToMouse() = 0;
        virtual void onSelectionChange(bool UnitsDeselected, bool UnitsSelected) = 0;

        std::set<fi::Slot_Map_ID> Selected;
        virtual void deselectUnit(fi::Slot_Map_ID ID);
        virtual void deselectUnits();
        virtual void selectUnit(fi::Slot_Map_ID ID);
        virtual void selectUnits(std::vector<fi::Slot_Map_ID> Units);
        virtual void selectNextUnit();
        virtual void selectPreviousUnit();
		virtual bool isUnitSelected(fi::Slot_Map_ID ID);
		virtual bool isUnitSelected();

        std::vector<std::vector<fi::Slot_Map_ID>> ControlGroups;
        virtual void controlGroupToUnitsSelected(int ControlGroup);
        virtual void unitsSelectedToControlGroup_OverwriteList(int ControlGroup);
        virtual void unitsSelectedToControlGroup_AppendToList(int ControlGroup);
        virtual void removeFromControlGroups(fi::Slot_Map_ID UnitsArrayIndex);

        //std::vector<Selection_Queue_Item> SelectionQueue;

        bool UnitsSelectedThisFrame = false;
        bool UnitsDeselectedThisFrame = false;
        const sf::Vector2i SELECTION_BOX_INACTIVE_COORDS = sf::Vector2i(-1, -1);
        sf::Vector2i BoxSelectInitialMouseDownPosition = SELECTION_BOX_INACTIVE_COORDS;
        void resetSelectionBox();
        bool isSelectionBoxActive();
        virtual bool onActionButtonReleased(bool AddToQueue);
    };
}

#endif