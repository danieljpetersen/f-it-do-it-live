#ifndef FI_RANDOM_H
#define FI_RANDOM_H

#include <random>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System.hpp>

namespace fi
{
	class Random_Generator
	{
	private:
		unsigned int Seed = 0;
        std::uniform_real_distribution<float> genI;
        std::uniform_real_distribution<float> genF;

    public:
		Random_Generator()
		{
			seed();
		}
		
		std::mt19937 Rand;
	
		unsigned int seed();
		unsigned int seed(unsigned int Value);
		unsigned int getCurrentSeed();
	
		bool chance();
		bool chance(int Percent); // out of 100
		bool chance(int Low, int High);
		int i(int Min, int Max); // including min, excluding max
		float f(float Min, float Max); // including min, excluding max
		sf::Color color();
		sf::Color color(sf::Color mix);
		std::string string(int Length);
        sf::Vector3f vector3f(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax);
        sf::Vector3f vector3f(float Min, float Max);
		sf::Vector2f vector2f(float Min, float Max);
		sf::Vector2f vector2f(float xMin, float xMax, float yMin, float yMax);
		sf::Vector2f pointInRadius(sf::Vector2f CenterPosition, float Radius);
	};

    fi::Random_Generator &getRandom();
}


#endif