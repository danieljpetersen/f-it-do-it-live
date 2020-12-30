#include "color_gradient.h"
#include "fi_math.h"
#include "engine.h"

////////////////////////////////////////////////////////////

int fi::Color_Gradient::DefaultWidth = 10;

////////////////////////////////////////////////////////////

void fi::Color_Gradient::init(sf::Color a, sf::Color b)
{
	init(a, b, DefaultWidth);
}

////////////////////////////////////////////////////////////

void fi::Color_Gradient::init(sf::Color a, sf::Color b, int GradientWidth)
{
	this->a = a;
	this->b = b;

	Gradient.clear();
	Gradient.resize(GradientWidth);

	for (int i = 0; i < GradientWidth; i++)
	{
        float p = i / float(GradientWidth - 1);
        Gradient[i].r = int((1.0-p) * a.r + p * b.r + 0.5);
        Gradient[i].g = int((1.0-p) * a.g + p * b.g + 0.5);
        Gradient[i].b = int((1.0-p) * a.b + p * b.b + 0.5);
        Gradient[i].a = int((1.0-p) * a.a + p * b.a + 0.5);
        Gradient[i].a = 255;
	}
}

////////////////////////////////////////////////////////////

sf::Color fi::Color_Gradient::getColor(float Normalized)
{
	int Index = Normalized * (Gradient.size()-1);

	if (Index < 0)
	{
		Index = 0;
	}
	if (Index >= Gradient.size())
	{
		Index = Gradient.size() - 1;
	}
	return Gradient[Index];
}

////////////////////////////////////////////////////////////

sf::Color fi::Color_Gradient::getColor(float Value, float min, float max)
{
	return getColor(fi::Math::normalize(Value, min, max));
}

////////////////////////////////////////////////////////////

sf::Color fi::Color_Gradient::getRandomColorFromGradient()
{
	int Index = fi::getRandom().i(0, (int)Gradient.size());
	return Gradient[Index];
}