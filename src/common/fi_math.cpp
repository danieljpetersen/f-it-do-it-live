#include <cmath>
#include <math.h>
#include "fi_math.h"

////////////////////////////////////////////////////////////

float fi::Math::normalize(float value, float min, float max)
{
    auto denominator= max - min;
    if (denominator == 0)
        return 1.0f;

    return (value - min) / (max - min);
}

////////////////////////////////////////////////////////////

float fi::Math::distance(sf::Vector2f a, sf::Vector2f b)
{
    return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

////////////////////////////////////////////////////////////

float fi::Math::getPercent(float Percent, float Number)
{
    return (Percent / 100) * Number;
}

////////////////////////////////////////////////////////////

sf::Vector2f fi::Math::infiniteLineCast_PointOfIntersection(sf::Vector2f aLineStart, sf::Vector2f aLineEnd, sf::Vector2f bLineStart, sf::Vector2f bLineEnd)
{
    sf::Vector2f Result;

    double denominator, a, b, numerator1, numerator2;
    denominator = ((bLineEnd.y - bLineStart.y) * (aLineEnd.x - aLineStart.x)) - ((bLineEnd.x - bLineStart.x) * (aLineEnd.y - aLineStart.y));

    a = aLineStart.y - bLineStart.y;
    b = aLineStart.x - bLineStart.x;
    numerator1 = ((bLineEnd.x - bLineStart.x) * a) - ((bLineEnd.y - bLineStart.y) * b);
    numerator2 = ((aLineEnd.x - aLineStart.x) * a) - ((aLineEnd.y - aLineStart.y) * b);

    if (denominator == 0)
    {
        return sf::Vector2f(0,0);
    }
    else
    {
        a = numerator1 / denominator;
        b = numerator2 / denominator;
    }

    // todo -- divide by 0 possible???

    // if we cast these lines infinitely in both directions, they intersect here:
    Result.x = aLineStart.x + (a * (aLineEnd.x - aLineStart.x));
    Result.y = aLineStart.y + (a * (aLineEnd.y - aLineStart.y));
    return Result;
}

////////////////////////////////////////////////////////////

bool fi::Math::doLineSegmentsIntersect(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f q1, sf::Vector2f q2, sf::Vector2f &PointOfIntersection)
{
    if (doLineSegmentsIntersect(p1, p2, q1, q2))
    {
        // todo -- oh my god what the fuck todo
        PointOfIntersection = infiniteLineCast_PointOfIntersection(p1, p2, q1, q2);
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////

bool fi::Math::doLineSegmentsIntersect(sf::Vector2f Point1, sf::Vector2f Point2, sf::Vector2f Point3, sf::Vector2f Point4)
{
    float bx = Point2.x - Point1.x;
    float by = Point2.y - Point1.y;
    float dx = Point4.x - Point3.x;
    float dy = Point4.y - Point3.y;
    float b_dot_d_perp = bx * dy - by * dx;
    if (b_dot_d_perp == 0)
    {
        return false;
    }

    float cx = Point3.x - Point1.x;
    float cy = Point3.y - Point1.y;
    float t = (cx * dy - cy * dx) / b_dot_d_perp;

    if (t <= 0 || t >= 1)
    {
        return false;
    }
    float u = (cx * by - cy * bx) / b_dot_d_perp;

    if (u <= 0 || u >= 1)
    {
        return false;
    }

    return true;
}
