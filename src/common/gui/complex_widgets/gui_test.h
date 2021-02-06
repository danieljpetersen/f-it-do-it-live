#ifndef FI_GUI_TEST_H
#define FI_GUI_TEST_H

#include <string>
#include <SFML/Graphics/Color.hpp>

namespace fi
{
	////////////////////////////////////////////////////////////
	// test gui todo delete eventually
    class GUI_Test
	{
	public:
		void guiTest();

		int SelectedVerticalTest = 0;
		std::string inputText;
		bool ShowingProgressBarForTickCounter = false;
		float SliderValue = 0.3f;
		sf::Color ColorPickerTest;
		int PointBarChangeCount = -1;
	};
}

#endif