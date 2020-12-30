#include "color.h"
#include "engine.h"
#include <sstream>
#include <cmath>

namespace fi
{
	sf::Color colorBlend(sf::Color a, sf::Color b)
	{
		std::vector<sf::Color> Colors;
		Colors.push_back(a);
		Colors.push_back(b);
		return colorBlend(Colors);
	}

	////////////////////////////////////////////////////////////

	sf::Color colorBlend(std::vector<sf::Color> Colors)
	{
		int r = 0, b = 0, g = 0, a = 0;
		for (unsigned int i = 0; i < Colors.size(); i++)
		{
			r += (int)Colors[i].r * (int)Colors[i].r;
			g += (int)Colors[i].g * (int)Colors[i].g;
			b += (int)Colors[i].b * (int)Colors[i].b;
			a += (int)Colors[i].a * (int)Colors[i].a;
		}
		r = (sf::Uint8)(sqrt((double)(r / Colors.size())));
		g = (sf::Uint8)(sqrt((double)(g / Colors.size())));
		b = (sf::Uint8)(sqrt((double)(b / Colors.size())));
		a = (sf::Uint8)(sqrt((double)(a / Colors.size())));

		return sf::Color(r, g, b, a);
	}

	////////////////////////////////////////////////////////////

	sf::Color colorAverage(sf::Color a, sf::Color b)
	{
		sf::Color NewColor;
		NewColor.r = (a.r + b.r) / 2;
		NewColor.g = (a.g + b.g) / 2;
		NewColor.b = (a.b + b.b) / 2;
		NewColor.a = (a.a + b.a) / 2;
		return NewColor;
	}
	
	////////////////////////////////////////////////////////////
	
	sf::Color colorModify(sf::Color Color, int rgbChange)
	{
		return colorModify(Color, rgbChange, rgbChange, rgbChange, 0);
	}
	////////////////////////////////////////////////////////////
	
	sf::Color colorModifyRandom(sf::Color Color, int rgbChange)
	{
		return colorModify(Color, fi::getRandom().i(-rgbChange, rgbChange), fi::getRandom().i(-rgbChange, rgbChange), fi::getRandom().i(-rgbChange, rgbChange), 0);
	}
	
	////////////////////////////////////////////////////////////

	sf::Color colorModifyRandom(sf::Color Color, int rChange, int gChange, int bChange, int aChange)
	{
		return colorModify(Color, fi::getRandom().i(-rChange, rChange), fi::getRandom().i(-gChange, gChange), fi::getRandom().i(-bChange, bChange), fi::getRandom().i(-aChange, aChange));
	}
	
	////////////////////////////////////////////////////////////

	sf::Color colorModify(sf::Color Color, int rChange, int gChange, int bChange, int aChange)
	{
		int r = Color.r;
		int g = Color.g;
		int b = Color.b;
		int a = Color.a;

		r += rChange;
		g += gChange;
		b += bChange;
		a += aChange;

		if (r < 0)
			r = 0;
		if (g < 0)
			g = 0;
		if (b < 0)
			b = 0;
		if (a < 0)
			a = 0;
		if (r > 255)
			r = 255;
		if (g > 255)
			g = 255;
		if (b > 255)
			b = 255;
		if (a > 255)
			a = 255;

		return sf::Color(r, g, b, a);
	}

	////////////////////////////////////////////////////////////
	//lower score is the closer score.  can be around 80 for pretty close to 140 for not as close to 300+ for pretty far
	double colorRankCloseness(sf::Color e1, sf::Color e2)
	{
		long rmean = ((long)e1.r + (long)e2.r) / 2;
		long r = (long)e1.r - (long)e2.r;
		long g = (long)e1.g - (long)e2.g;
		long b = (long)e1.b - (long)e2.b;
		return sqrt((((512 + rmean)*r*r) >> 8) + 4 * g*g + (((767 - rmean)*b*b) >> 8));
	}

	////////////////////////////////////////////////////////////

	int colorRankCloseness(sf::Color Color, std::vector<sf::Color> Colors)
	{
		int ClosestIndex = -1, ClosestRank = -1;
		for (int i = 0; i < Colors.size(); i++)
		{
			int Value = fi::colorRankCloseness(Color, Colors[i]);
			if ((Value < ClosestRank) || (ClosestIndex == -1))
			{
				ClosestRank = Value;
				ClosestIndex = i;
			}
		}

		return ClosestIndex;
	}

	////////////////////////////////////////////////////////////

	sf::Color colorInvert(sf::Color Color)
	{
		Color.r = sf::Uint8(255 - Color.r);
		Color.g = sf::Uint8(255 - Color.g);
		Color.b = sf::Uint8(255 - Color.b);
		return Color;
	}

	////////////////////////////////////////////////////////////

	double colorConvertToGreyscale(sf::Color Color)
	{
		float r = Color.r;
		float g = Color.g;
		float b = Color.b;
		return  r*0.299 + g*0.587 + b*0.114;
	}

    ////////////////////////////////////////////////////////////

    sf::Color colorConvertToGreyscale_ReturnColor(sf::Color Color)
	{
		sf::Uint8 value = sf::Uint8(colorConvertToGreyscale(Color));
		Color.r = value;
		Color.g = value;
		Color.b = value;
		return Color;
	}

    ////////////////////////////////////////////////////////////

    const int GreyscaleThreshold = 140;
	
	sf::Color colorGetBlackOrWhite(sf::Color Color)
	{
		double greyscale = colorConvertToGreyscale(Color);
		if (greyscale < GreyscaleThreshold)//traditioanlly is 186
		{
			return sf::Color::White;
		}
		return sf::Color::Black;
	}

    ////////////////////////////////////////////////////////////

    bool colorGetBlackOrWhite_BooleanTrueForBlack(sf::Color Color)
	{
		double greyscale = colorConvertToGreyscale(Color);
		if (greyscale < GreyscaleThreshold)//traditioanlly is 186
		{
			return false;
		}
		
		return true;
	}

    ////////////////////////////////////////////////////////////

    void colorConvertToRGB(std::string HexCode, sf::Color &Color)
	{
		if(HexCode.at(0) == '#') {
			HexCode = HexCode.erase(0, 1);
		}

		int r, g, b;

		// ... and extract the rgb values.
		std::istringstream(HexCode.substr(0,2)) >> std::hex >> r;
		std::istringstream(HexCode.substr(2,2)) >> std::hex >> g;
		std::istringstream(HexCode.substr(4,2)) >> std::hex >> b;

		Color.r = (sf::Uint8)r;
		Color.g = (sf::Uint8)g;
		Color.b = (sf::Uint8)b;
		Color.a = 255;
	}

    ////////////////////////////////////////////////////////////

	sf::Color colorFromHSB(float Hue, float Saturation, float Brightness)
	{
		// todo

        return sf::Color::Black;
	}

    ////////////////////////////////////////////////////////////

    sf::Color colorLerp(sf::Color a, sf::Color b, float Normalized)
	{
		// todo

		return sf::Color::Black;
	}

    ////////////////////////////////////////////////////////////

    void HSB::fromColor(sf::Color Color)
	{
	    // todo
	}
}
