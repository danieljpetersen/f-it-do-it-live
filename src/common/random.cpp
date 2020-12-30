#include <chrono>
#include <algorithm>
#include "random.h"

namespace fi
{
	unsigned int Random_Generator::seed()
	{
		return seed((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
	}

    ////////////////////////////////////////////////////////////

	unsigned int Random_Generator::seed(unsigned int Value)
	{
		this->Seed = Value;
		srand(Seed);
        Rand.seed(Value);
        return Seed;
	}

    ////////////////////////////////////////////////////////////

	unsigned int Random_Generator::getCurrentSeed()
	{
		return Seed;
	}

    ////////////////////////////////////////////////////////////

	int Random_Generator::i(int Min, int Max) // including min, excluding max
	{
        std::uniform_real_distribution<float> gen(Min, Max); // uniform, unbiased
        return (int)gen(Rand);
	}

    ////////////////////////////////////////////////////////////

	float Random_Generator::f(float Min, float Max) // including min, excluding max
	{
        std::uniform_real_distribution<float> gen(Min, Max); // uniform, unbiased
        return gen(Rand);
    }

    ////////////////////////////////////////////////////////////

	bool Random_Generator::chance()
	{
        return i(0, 100) <= 50;
    }

    ////////////////////////////////////////////////////////////

	bool Random_Generator::chance(int Percent)
	{
        return i(0, 100) <= Percent;
    }

    ////////////////////////////////////////////////////////////

	bool Random_Generator::chance(int low, int high)
	{
        return i(low, high) <= low;
    }

    ////////////////////////////////////////////////////////////

	sf::Color Random_Generator::color()
	{
		return sf::Color(i(0, 226), i(0, 226), i(0, 226));
	}

    ////////////////////////////////////////////////////////////

	sf::Color Random_Generator::color(sf::Color mix)
	{
		sf::Color random = color();

		int red = (random.r + mix.r) / 2;
		int green = (random.g + mix.g) / 2;
		int blue = (random.b + mix.b) / 2;

		sf::Color color(red, green, blue);
		return color;
	}

    ////////////////////////////////////////////////////////////

	std::string Random_Generator::string(int length)
	{
		auto randchar = []() -> char
		{
			const char charset[] =
					"0123456789"
							"EFGHIJKLMNOPQRSTUVWXYZ"
							"abcdefghijklmnopqrstuvwxyz";
			const size_t max_index = (sizeof(charset) - 1);
			return charset[ rand() % max_index ];
		};
		std::string str(length,0);
		std::generate_n( str.begin(), length, randchar );
		return str;
	}

    ////////////////////////////////////////////////////////////

	sf::Vector3f Random_Generator::vector3f(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax)
	{
		return sf::Vector3f(f(xMin,xMax), f(xMin,xMax), f(zMin, zMax));
	}

    	////////////////////////////////////////////////////////////

	sf::Vector3f Random_Generator::vector3f(float min, float max)
	{
		return sf::Vector3f(f(min, max), f(min, max), f(min, max));
	}

    ////////////////////////////////////////////////////////////

	sf::Vector2f Random_Generator::vector2f(float min, float max)
	{
		return sf::Vector2f(f(min, max), f(min, max));
	}

    ////////////////////////////////////////////////////////////

    sf::Vector2f Random_Generator::vector2f(float xMin, float xMax, float yMin, float yMax)
    {
	    return sf::Vector2f(f(xMin,xMax), f(yMin, yMax));
    }

    ////////////////////////////////////////////////////////////

	sf::Vector2f Random_Generator::pointInRadius(sf::Vector2f CenterPosition, float Radius)
	{
        float Angle = i(0, 360); // between 0 and 360 (degrees)
        float x = CenterPosition.x + Radius*cos(Angle);
        float y = CenterPosition.y + Radius*sin(Angle);

        return sf::Vector2f(x, y);
	}
}
