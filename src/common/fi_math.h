#ifndef SUB_MATH_HELPER_FUNCTIONS_H
#define SUB_MATH_HELPER_FUNCTIONS_H

#include <SFML/Graphics.hpp>
#include <math.h>

#ifndef  M_PI
#define M_PI		3.14159265358979323846	/* pi */
#endif

#include <cmath>
#include <assert.h>

namespace fi
{
    namespace Math
    {
        float normalize(float value, float min, float max);
        float distance(sf::Vector2f a, sf::Vector2f b);
        float getPercent(float Percent, float Number);


        sf::Vector2f infiniteLineCast_PointOfIntersection(sf::Vector2f aLineStart, sf::Vector2f aLineEnd, sf::Vector2f bLineStart, sf::Vector2f bLineEnd);
        bool doLineSegmentsIntersect(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f q1, sf::Vector2f q2, sf::Vector2f &PointOfIntersection);
        bool doLineSegmentsIntersect(sf::Vector2f Point1, sf::Vector2f Point2, sf::Vector2f Point3, sf::Vector2f Point4);
    }
}

#endif