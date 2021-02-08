#ifndef FI_CONTROLS_H
#define FI_CONTROLS_H

#include <unordered_map>
#include "../common/plugins.h"
#include "../common/tick.h"
#include "../common/gui/complex_widgets/gui_tick_controller.h"
#include "../common/selection_manager.h"
#include "pathfinding.h"

namespace IUEINW
{
	class IUEINW_Unit_Selection_Manager : public fi::Selection_Manager
	{
	public:
		void onDraw() override;
		void performUnitMoveCommand() override;
		std::vector<fi::Slot_Map_ID> getUnitsInRect(sf::Vector2f SelectionTopLeft, int SelectionWidth, int SelectionHeight) override;
		fi::Slot_Map_ID getUnitClosestToMouse() override;
		void onSelectionChange(bool UnitsDeselected, bool UnitsSelected) override;
		std::set<int> getSelectedTiles();
	};

	////////////////////////////////////////////////////////////

	class IUEINW_Plugin_User_Interface : public fi::Plugin_Base
    {
    public:
        void work(const int Event) override;
        void onEnableChange(bool Enabling) override;
        void reset();

		IUEINW_Unit_Selection_Manager SelectionManager;
		std::unordered_map<fi::Slot_Map_ID, IUEINW_Path, fi::Slot_Map_Hash_Fn> Paths;

    private:
    	void onUpdate();
        void onDraw();
        void guiCore();

		fi::Tick_Controller TickController;
	};

	////////////////////////////////////////////////////////////

	IUEINW_Plugin_User_Interface &getUI();
}

#endif