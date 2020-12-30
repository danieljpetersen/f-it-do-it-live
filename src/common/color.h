#ifndef FI_COLOR_H
#define FI_COLOR_H

#include <SFML/Graphics.hpp>
#include "color_gradient.h"

namespace fi
{
    namespace Color // extra colors, why not
    {
        const sf::Color Red = sf::Color::Red;
        const sf::Color Green = sf::Color::Green;
        const sf::Color Blue = sf::Color::Blue;
        const sf::Color Black = sf::Color::Black;
        const sf::Color White = sf::Color::White;
        const sf::Color Yellow = sf::Color::Yellow;
        const sf::Color Cyan = sf::Color::Cyan;
        const sf::Color Magenta = sf::Color::Magenta;
        const sf::Color Transparent = sf::Color::Transparent;

        const sf::Color LightBlue = sf::Color (173, 216, 230);
        const sf::Color SkyBlue = sf::Color (135, 206, 235);
        const sf::Color DarkBlue = sf::Color(0, 0, 139);
        const sf::Color Pink = sf::Color(255, 192, 203);
        const sf::Color Orange = sf::Color(255, 165, 0);
        const sf::Color Purple = sf::Color(160, 32, 240);
        const sf::Color Violet = sf::Color(238, 130, 238);
        const sf::Color Darkgreen = sf::Color(0, 100, 0);
        const sf::Color LightGreen = sf::Color(144, 238, 144);
        const sf::Color Gray = sf::Color (190, 190, 190);
        const sf::Color Brown = sf::Color(139,69,19);
    }

    void colorConvertToRGB(std::string Hex, sf::Color &Color);
    double colorConvertToGreyscale(sf::Color Color);
    double colorRankCloseness(sf::Color e1, sf::Color e2); 	//lower score is the closer score.  can be around 80 for pretty close to 140 for not as close to 300+ for pretty far
    bool colorGetBlackOrWhite_BooleanTrueForBlack(sf::Color Color);
    int colorRankCloseness(sf::Color Color, std::vector<sf::Color> Choices);
    sf::Color colorConvertToGreyscale_ReturnColor(sf::Color Color);
    sf::Color colorGetBlackOrWhite(sf::Color Color);
    sf::Color colorInvert(sf::Color Color);
    sf::Color colorBlend(sf::Color a, sf::Color b);
    sf::Color colorBlend(std::vector<sf::Color> Colors);
    sf::Color colorAverage(sf::Color a, sf::Color b);
    sf::Color colorModifyRandom(sf::Color Color, int rgbChange);
    sf::Color colorModifyRandom(sf::Color Color, int rChange, int gChange, int bChange, int aChange);
    sf::Color colorModify(sf::Color Color, int rChange, int gChange, int bChange, int aChange);
    sf::Color colorModify(sf::Color Color, int rgbChange);
    sf::Color colorLerp(sf::Color a, sf::Color b, float Normalized);

    // ----

    // todo HSB stuff not yet implemented

    sf::Color colorFromHSB(float Hue, float Saturation, float Brightness);

    class HSB
    {
    public:
        float Hue;
        float Saturation;
        float Brightness;

        HSB() = default;

        HSB(int Hue, int Saturation, int Brightness)
        {
            this->Hue = Hue;
            this->Saturation = Saturation;
            this->Brightness = Brightness;
        }

        HSB(sf::Color Color)
        {
            this->fromColor(Color);
        }

        sf::Color toColor()
        {
            return colorFromHSB(Hue, Saturation, Brightness);
        }

        void fromColor(sf::Color Color);
    };
}

#endif