#ifndef FI_COLOR_GRADIENT_H
#define FI_COLOR_GRADIENT_H

#include <vector>
#include <SFML/Graphics.hpp>

namespace fi
{
	class Color_Gradient
	{
	public:
        void init(sf::Color a, sf::Color b);
        void init(sf::Color a, sf::Color b, int GradientWidth);
		sf::Color getColor(float Normalized);
		sf::Color getColor(float Value, float min, float max);
		sf::Color getRandomColorFromGradient();

		static int DefaultWidth;

		sf::Color a;
		sf::Color b;
		std::vector<sf::Color> Gradient;
	};
}

#endif