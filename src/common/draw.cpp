#include <cmath>
#include "draw.h"
#include "engine.h"

void fi::Draw::text(std::string Str, int x, int y)
{
    sf::Text T;
    T.setFont(fi::getEngine().GUI.font);
    T.setString(Str);
    T.setPosition(x, y);
    fi::getCanvasGUI().draw(T);
}

////////////////////////////////////////////////////////////

void fi::Draw::text(sf::RenderTarget *RenderTarget, std::string Str, int x, int y)
{
    sf::Text T;
    T.setFont(fi::getEngine().GUI.font);
    T.setString(Str);
    T.setPosition(x, y);
    RenderTarget->draw(T);
}

////////////////////////////////////////////////////////////

void fi::Draw::line(sf::RenderTarget *RenderTarget, sf::Vector2f a, sf::Vector2f b, sf::Color ColorToDraw)
{
    sf::VertexArray lines(sf::Lines, 2);
    lines[0].position = a;
    lines[0].color = ColorToDraw;
    lines[1].position = b;
    lines[1].color = ColorToDraw;

    RenderTarget->draw(lines);
}

////////////////////////////////////////////////////////////

void fi::Draw::line(sf::RenderTarget *RenderTarget, sf::Vector2i a, sf::Vector2i b, sf::Color ColorToDraw)
{
    sf::VertexArray lines(sf::Lines, 2);
    lines[0].position = sf::Vector2f(a);
    lines[0].color = ColorToDraw;
    lines[1].position = sf::Vector2f(b);
    lines[1].color = ColorToDraw;

    RenderTarget->draw(lines);
}

////////////////////////////////////////////////////////////

void fi::Draw::line(sf::VertexArray &vertexArray, sf::Vector2f a, sf::Vector2f b, sf::Color ColorToDraw)
{
    sf::Vertex vertex;
    vertex.color = ColorToDraw;
    vertex.position = a;
    vertexArray.append(vertex);

    vertex.position = b;
    vertexArray.append(vertex);
}

////////////////////////////////////////////////////////////

void fi::Draw::line(sf::RenderTarget *RenderTarget, sf::Vector2i a, sf::Vector2i b, sf::Color ColorToDraw, sf::Transform Transformation)
{
    sf::VertexArray lines(sf::Lines, 2);
    lines[0].position = sf::Vector2f(a);
    lines[0].color = ColorToDraw;
    lines[1].position = sf::Vector2f(b);
    lines[1].color = ColorToDraw;

    RenderTarget->draw(lines, Transformation);
}

////////////////////////////////////////////////////////////

void fi::Draw::line(sf::RenderTarget *RenderTarget, sf::Vector2i a, sf::Vector2i b, sf::Color ColorToDraw, float Thickness)
{
    sf::Vector2f Normal((float) a.y - (float) b.y, (float) b.x - (float) a.x);
    float Length = std::sqrt(Normal.x * Normal.x + Normal.y * Normal.y);
    if (Length != 0.f)
        Normal /= Length;

    Normal = Normal * Thickness;
    Normal = Normal / 2.0f;

    sf::VertexArray Line(sf::Quads);
    sf::Vertex Vert[4] =
            {
                    sf::Vertex(sf::Vector2f(a) - Normal, ColorToDraw),
                    sf::Vertex(sf::Vector2f(b) - Normal, ColorToDraw),
                    sf::Vertex(sf::Vector2f(b) + Normal, ColorToDraw),
                    sf::Vertex(sf::Vector2f(a) + Normal, ColorToDraw)
            };

    Line.append(Vert[0]);
    Line.append(Vert[1]);
    Line.append(Vert[2]);
    Line.append(Vert[3]);

    RenderTarget->draw(Line);
}

////////////////////////////////////////////////////////////

void fi::Draw::line(sf::RenderTarget *RenderTarget, sf::Vector2f a, sf::Vector2f b, sf::Color ColorToDraw, float Thickness)
{
    sf::VertexArray Line(sf::Quads);
    line(Line, a, b, ColorToDraw, Thickness);
    RenderTarget->draw(Line);
}

////////////////////////////////////////////////////////////

void fi::Draw::line(std::vector<sf::Vertex> &VertexBuffer, sf::Vector2f a, sf::Vector2f b, sf::Color ColorToDraw)
{
    line(VertexBuffer, a, b, ColorToDraw, 1);
}

////////////////////////////////////////////////////////////

void fi::Draw::line(std::vector<sf::Vertex> &VertexBuffer, sf::Vector2f a, sf::Vector2f b, sf::Color ColorToDraw, float Thickness)
{
    sf::Vector2f Normal(a.y - b.y, b.x - a.x);
    float Length = std::sqrt(Normal.x * Normal.x + Normal.y * Normal.y);
    if (Length != 0.f)
        Normal /= Length;

    Normal = Normal * Thickness;
    Normal = Normal / 2.0f;

    sf::Vertex Vert[4] =
            {
                    sf::Vertex(a - Normal, ColorToDraw),
                    sf::Vertex(b - Normal, ColorToDraw),
                    sf::Vertex(b + Normal, ColorToDraw),
                    sf::Vertex(a + Normal, ColorToDraw)
            };

    VertexBuffer.push_back(Vert[0]);
    VertexBuffer.push_back(Vert[1]);
    VertexBuffer.push_back(Vert[2]);
    VertexBuffer.push_back(Vert[3]);
}

////////////////////////////////////////////////////////////

void fi::Draw::line(sf::VertexArray &Line, sf::Vector2f a, sf::Vector2f b, sf::Color ColorToDraw, float Thickness)
{
    sf::Vector2f Normal(a.y - b.y, b.x - a.x);
    float Length = std::sqrt(Normal.x * Normal.x + Normal.y * Normal.y);
    if (Length != 0.f)
        Normal /= Length;

    Normal = Normal * Thickness;
    Normal = Normal / 2.0f;

    sf::Vertex Vert[4] =
            {
                    sf::Vertex(a - Normal, ColorToDraw),
                    sf::Vertex(b - Normal, ColorToDraw),
                    sf::Vertex(b + Normal, ColorToDraw),
                    sf::Vertex(a + Normal, ColorToDraw)
            };

    Line.append(Vert[0]);
    Line.append(Vert[1]);
    Line.append(Vert[2]);
    Line.append(Vert[3]);
}

////////////////////////////////////////////////////////////

void fi::Draw::line(sf::VertexArray &VertexArray, sf::Vector2i a, sf::Vector2i b, sf::Color ColorToDraw, float Thickness)
{
    sf::Vector2f Normal((float) a.y - (float) b.y, (float) b.x - (float) a.x);
    float Length = std::sqrt(Normal.x * Normal.x + Normal.y * Normal.y);
    if (Length != 0.f)
        Normal /= Length;

    Normal = Normal * Thickness;
    Normal = Normal / 2.0f;

    sf::Vertex Vert[4] =
            {
                    sf::Vertex(sf::Vector2f(a) - Normal, ColorToDraw),
                    sf::Vertex(sf::Vector2f(b) - Normal, ColorToDraw),
                    sf::Vertex(sf::Vector2f(b) + Normal, ColorToDraw),
                    sf::Vertex(sf::Vector2f(a) + Normal, ColorToDraw)
            };

    VertexArray.append(Vert[0]);
    VertexArray.append(Vert[1]);
    VertexArray.append(Vert[2]);
    VertexArray.append(Vert[3]);
}

////////////////////////////////////////////////////////////

void fi::Draw::line(sf::VertexArray &VertexArray, sf::Vector2i a, sf::Vector2i b, sf::Color aColorToDraw, sf::Color bColorToDraw, float Thickness)
{
    sf::Vector2f Normal((float) a.y - (float) b.y, (float) b.x - (float) a.x);
    float Length = std::sqrt(Normal.x * Normal.x + Normal.y * Normal.y);
    if (Length != 0.f)
        Normal /= Length;

    Normal = Normal * Thickness;
    Normal = Normal / 2.0f;

    sf::Vertex Vert[4] =
            {
                    sf::Vertex(sf::Vector2f(a) - Normal, aColorToDraw),
                    sf::Vertex(sf::Vector2f(b) - Normal, aColorToDraw),
                    sf::Vertex(sf::Vector2f(b) + Normal, bColorToDraw),
                    sf::Vertex(sf::Vector2f(a) + Normal, bColorToDraw)
            };

    VertexArray.append(Vert[0]);
    VertexArray.append(Vert[1]);
    VertexArray.append(Vert[2]);
    VertexArray.append(Vert[3]);
}

////////////////////////////////////////////////////////////

void fi::Draw::line(sf::RenderTarget *RenderTarget, sf::Vector2i a, sf::Vector2i b, sf::Color ColorToDraw, float Thickness, sf::Transform Transformation)
{
    sf::Vector2f Normal((float) a.y - (float) b.y, (float) b.x - (float) a.x);
    float Length = std::sqrt(Normal.x * Normal.x + Normal.y * Normal.y);
    if (Length != 0.f)
        Normal /= Length;

    Normal = Normal * Thickness;
    Normal = Normal / 2.0f;

    sf::VertexArray Line(sf::Quads);
    sf::Vertex Vert[4] =
            {
                    sf::Vertex(sf::Vector2f(a) - Normal, ColorToDraw),
                    sf::Vertex(sf::Vector2f(b) - Normal, ColorToDraw),
                    sf::Vertex(sf::Vector2f(b) + Normal, ColorToDraw),
                    sf::Vertex(sf::Vector2f(a) + Normal, ColorToDraw)
            };

    Line.append(Vert[0]);
    Line.append(Vert[1]);
    Line.append(Vert[2]);
    Line.append(Vert[3]);

    RenderTarget->draw(Line, Transformation);
}

////////////////////////////////////////////////////////////

void fi::Draw::line(sf::RenderTarget *RenderTarget, sf::Vector2i a, sf::Vector2i b, sf::Color aColorToDraw, sf::Color bColorToDraw, float Thickness)
{
    sf::Vector2f Normal((float) a.y - (float) b.y, (float) b.x - (float) a.x);
    float Length = std::sqrt(Normal.x * Normal.x + Normal.y * Normal.y);
    if (Length != 0.f)
        Normal /= Length;

    Normal = Normal * Thickness;
    Normal = Normal / 2.0f;

    sf::VertexArray Line(sf::Quads);
    sf::Vertex Vert[4] =
            {
                    sf::Vertex(sf::Vector2f(a) - Normal, aColorToDraw),
                    sf::Vertex(sf::Vector2f(b) - Normal, aColorToDraw),
                    sf::Vertex(sf::Vector2f(b) + Normal, bColorToDraw),
                    sf::Vertex(sf::Vector2f(a) + Normal, bColorToDraw)
            };


    Line.append(Vert[0]);
    Line.append(Vert[1]);
    Line.append(Vert[2]);
    Line.append(Vert[3]);

    RenderTarget->draw(Line);
}

////////////////////////////////////////////////////////////

void rect(std::vector<sf::Vertex> &vertexBuffer, sf::Vector2f Position, int Width, int Height, sf::Color ColorToDraw)
{
    vertexBuffer.push_back(sf::Vertex(sf::Vector2f(Position), ColorToDraw));
    vertexBuffer.push_back(sf::Vertex(sf::Vector2f(Position.x + Width, Position.y), ColorToDraw));
    vertexBuffer.push_back(sf::Vertex(sf::Vector2f(Position.x + Width, Position.y + Height), ColorToDraw));
    vertexBuffer.push_back(sf::Vertex(sf::Vector2f(Position.x, Position.y + Height), ColorToDraw));
}

////////////////////////////////////////////////////////////

void rect(std::vector<sf::Vertex> &vertexBuffer, sf::Vector2i Position, int Width, int Height, sf::Color ColorToDraw)
{
    vertexBuffer.push_back(sf::Vertex(sf::Vector2f(Position), ColorToDraw));
    vertexBuffer.push_back(sf::Vertex(sf::Vector2f(Position.x + Width, Position.y), ColorToDraw));
    vertexBuffer.push_back(sf::Vertex(sf::Vector2f(Position.x + Width, Position.y + Height), ColorToDraw));
    vertexBuffer.push_back(sf::Vertex(sf::Vector2f(Position.x, Position.y + Height), ColorToDraw));
}

////////////////////////////////////////////////////////////

void fi::Draw::rect(std::vector<sf::Vertex> &vertexBuffer, sf::Vector2f Position, int Width, int Height, sf::Color ColorToDraw)
{
    vertexBuffer.push_back(sf::Vertex(sf::Vector2f(Position), ColorToDraw));
    vertexBuffer.push_back(sf::Vertex(sf::Vector2f(Position.x + Width, Position.y), ColorToDraw));
    vertexBuffer.push_back(sf::Vertex(sf::Vector2f(Position.x + Width, Position.y + Height), ColorToDraw));
    vertexBuffer.push_back(sf::Vertex(sf::Vector2f(Position.x, Position.y + Height), ColorToDraw));
}

////////////////////////////////////////////////////////////

void fi::Draw::rect(std::vector<sf::Vertex> &vertexBuffer, sf::Vector2i Position, int Width, int Height, sf::Color ColorToDraw)
{
    vertexBuffer.push_back(sf::Vertex(sf::Vector2f(Position), ColorToDraw));
    vertexBuffer.push_back(sf::Vertex(sf::Vector2f(Position.x + Width, Position.y), ColorToDraw));
    vertexBuffer.push_back(sf::Vertex(sf::Vector2f(Position.x + Width, Position.y + Height), ColorToDraw));
    vertexBuffer.push_back(sf::Vertex(sf::Vector2f(Position.x, Position.y + Height), ColorToDraw));
}

////////////////////////////////////////////////////////////

void fi::Draw::rect(sf::VertexArray &vertexArray, sf::Vector2i Position, int Width, int Height, sf::Color ColorToDraw)
{
    vertexArray.append(sf::Vertex(sf::Vector2f(Position), ColorToDraw));
    vertexArray.append(sf::Vertex(sf::Vector2f(Position.x + Width, Position.y), ColorToDraw));
    vertexArray.append(sf::Vertex(sf::Vector2f(Position.x + Width, Position.y + Height), ColorToDraw));
    vertexArray.append(sf::Vertex(sf::Vector2f(Position.x, Position.y + Height), ColorToDraw));
}

////////////////////////////////////////////////////////////

void fi::Draw::rect(sf::VertexArray &vertexArray, sf::Vector2f Position, int Width, int Height, sf::Color ColorToDraw)
{
    vertexArray.append(sf::Vertex(Position, ColorToDraw));
    vertexArray.append(sf::Vertex(sf::Vector2f(Position.x + Width, Position.y), ColorToDraw));
    vertexArray.append(sf::Vertex(sf::Vector2f(Position.x + Width, Position.y + Height), ColorToDraw));
    vertexArray.append(sf::Vertex(sf::Vector2f(Position.x, Position.y + Height), ColorToDraw));
}

////////////////////////////////////////////////////////////

void fi::Draw::rect(sf::RenderTarget *RenderTarget, sf::Vector2i Position, int Width, int Height, sf::Color ColorToDraw)
{
    sf::Vertex rect[4] =
            {
                    sf::Vertex(sf::Vector2f(Position), ColorToDraw),
                    sf::Vertex(sf::Vector2f(Position.x + Width, Position.y), ColorToDraw),
                    sf::Vertex(sf::Vector2f(Position.x + Width, Position.y + Height), ColorToDraw),
                    sf::Vertex(sf::Vector2f(Position.x, Position.y + Height), ColorToDraw)
            };

    RenderTarget->draw(rect, 4, sf::Quads);
}

////////////////////////////////////////////////////////////

void fi::Draw::rect(sf::RenderTarget *RenderTarget, sf::Vector2f Position, int Width, int Height, sf::Color ColorToDraw)
{
    sf::Vertex rect[4] =
            {
                    sf::Vertex(sf::Vector2f(Position), ColorToDraw),
                    sf::Vertex(sf::Vector2f(Position.x + Width, Position.y), ColorToDraw),
                    sf::Vertex(sf::Vector2f(Position.x + Width, Position.y + Height), ColorToDraw),
                    sf::Vertex(sf::Vector2f(Position.x, Position.y + Height), ColorToDraw)
            };

    RenderTarget->draw(rect, 4, sf::Quads);
}

////////////////////////////////////////////////////////////

void fi::Draw::rect(sf::RenderTarget *RenderTarget, sf::Vector2i Position, int Width, int Height, sf::Color ColorToDraw, sf::Transform Transformation)
{
    sf::Vertex rect[4] =
            {
                    sf::Vertex(sf::Vector2f(Position), ColorToDraw),
                    sf::Vertex(sf::Vector2f(Position.x + Width, Position.y), ColorToDraw),
                    sf::Vertex(sf::Vector2f(Position.x + Width, Position.y + Height), ColorToDraw),
                    sf::Vertex(sf::Vector2f(Position.x, Position.y + Height), ColorToDraw)
            };

    RenderTarget->draw(rect, 4, sf::Quads, Transformation);
}

////////////////////////////////////////////////////////////

void fi::Draw::rectOutline(sf::VertexArray &VertexArray, sf::Vector2i TopLeftPosition, int Width, int Height, sf::Color ColorToDraw, sf::Transform Transformation)
{
    // todo
    //sf::Vector2f TopLeft2f = fi::convert(TopLeftPosition);
    //sf::Vector2f Position2 = TopLeft2f;
    //Position2.x += Width;
    //fi::fi::Draw::line(VertexArray, TopLeft2f, Position2, ColorToDraw, Transformation);
    //
    ////top-left to bottom-left
    //Position2 = TopLeftPosition;
    //Position2.y += Height;
    //line(VertexArray, TopLeft2f, Position2, ColorToDraw);
    //
    ////bottom-left to bottom-right
    //TopLeftPosition.x += Width;
    //TopLeftPosition.y += Height;
    //Position2 = TopLeftPosition;
    //Position2.x -= Width;
    //line(VertexArray, TopLeft2f, Position2, ColorToDraw);
    //
    //// top-right to bottom-right
    //Position2 = TopLeftPosition;
    //Position2.y -= Height;
    //line(VertexArray, TopLeft2f, Position2, ColorToDraw);
}

////////////////////////////////////////////////////////////

void fi::Draw::rectOutline(sf::VertexArray &VertexArray, sf::Vector2i TopLeftPosition, int Width, int Height, sf::Color ColorToDraw)
{
    rectOutline(VertexArray, fi::convert(TopLeftPosition), Width, Height, ColorToDraw);
}

////////////////////////////////////////////////////////////

void fi::Draw::rectOutline(sf::VertexArray &VertexArray, sf::Vector2f TopLeftPosition, int Width, int Height, sf::Color ColorToDraw)
{
    //top-left to top-right
    sf::Vector2f Position2 = TopLeftPosition;
    Position2.x += Width;
    fi::Draw::line(VertexArray, TopLeftPosition, Position2, ColorToDraw);

    //top-left to bottom-left
    Position2 = TopLeftPosition;
    Position2.y += Height;
    line(VertexArray, TopLeftPosition, Position2, ColorToDraw);

    //bottom-left to bottom-right
    TopLeftPosition.x += Width;
    TopLeftPosition.y += Height;
    Position2 = TopLeftPosition;
    Position2.x -= Width;
    line(VertexArray, TopLeftPosition, Position2, ColorToDraw);

    // top-right to bottom-right
    Position2 = TopLeftPosition;
    Position2.y -= Height;
    line(VertexArray, TopLeftPosition, Position2, ColorToDraw);
}

////////////////////////////////////////////////////////////

void fi::Draw::rectOutline(sf::RenderTarget *RenderTarget, sf::Vector2i Position, int Width, int Height, sf::Color ColorToDraw, sf::Transform Transformation)
{
    // top-left to top-right
    sf::Vector2i Position2 = Position;
    Position2.x += Width;
    line(RenderTarget, Position, Position2, ColorToDraw, Transformation);

    // top-left to bottom-left
    Position2 = Position;
    Position2.y += Height;
    line(RenderTarget, Position, Position2, ColorToDraw, Transformation);

    // bottom-left to bottom-right
    Position.x += Width;
    Position.y += Height;
    Position2 = Position;
    Position2.x -= Width;
    line(RenderTarget, Position, Position2, ColorToDraw, Transformation);

    // top-right to bottom-right
    Position2 = Position;
    Position2.y -= Height;
    line(RenderTarget, Position, Position2, ColorToDraw, Transformation);
}

////////////////////////////////////////////////////////////

void fi::Draw::rectOutline(sf::RenderTarget *RenderTarget, sf::Vector2i Position, int Width, int Height, sf::Color ColorToDraw)
{
    // top-left to top-right
    sf::Vector2i Position2 = Position;
    Position2.x += Width;
    line(RenderTarget, Position, Position2, ColorToDraw);

    // top-left to bottom-left
    Position2 = Position;
    Position2.y += Height;
    line(RenderTarget, Position, Position2, ColorToDraw);

    // bottom-left to bottom-right
    Position.x += Width ;
    Position.y += Height;
    Position2 = Position;
    Position2.x -= Width;
    line(RenderTarget, Position, Position2, ColorToDraw);

    // top-right to bottom-right
    Position2 = Position;
    Position2.y -= Height;
    line(RenderTarget, Position, Position2, ColorToDraw);
}

////////////////////////////////////////////////////////////

void fi::Draw::circle(std::vector<sf::Vertex> *VBO, int VBOStartIndex, int NumPoints, sf::Vector2f CenterPosition, float Radius, sf::Color Color)
{
    for (int i = 0; i < NumPoints; i++)
    {
        if (VBO->size() > VBOStartIndex + i + 3)
        {
            VBO->at(VBOStartIndex+i).position = CenterPosition;
            VBO->at(VBOStartIndex+i).color = Color;

            static const float pi = 3.141592654f;

            {
                float angle = (i+1) * 2 * pi / NumPoints - pi / 2;
                float x = std::cos(angle) * Radius;
                float y = std::sin(angle) * Radius;
                VBO->at(VBOStartIndex+i+1).position =  sf::Vector2f(Radius + x, Radius+ y);
                VBO->at(VBOStartIndex+i+1).color = Color;
            }

            {
                float angle = (i+2) * 2 * pi / NumPoints - pi / 2;
                float x = std::cos(angle) * Radius;
                float y = std::sin(angle) * Radius;
                VBO->at(VBOStartIndex+i+2).position = sf::Vector2f(Radius + x, Radius+ y);
                VBO->at(VBOStartIndex+i+2).color = Color;
            }
        }
    }


    // sf::CircleShape Test;
    // Test.setFillColor(color);
    // Test.setPosition(Position);
    // Test.setOrigin(sf::Vector2f(Radius, Radius));
    // Test.setRadius(Radius);
    //
    // Test.
    // RenderTarget->draw(Test);
}

////////////////////////////////////////////////////////////

void fi::Draw::circle(sf::RenderTarget *RenderTarget, sf::Vector2f Position, float Radius, sf::Color Color)
{
    sf::CircleShape CircleShape;
    CircleShape.setFillColor(Color);
    CircleShape.setPosition(Position);
    CircleShape.setOrigin(sf::Vector2f(Radius, Radius));
    CircleShape.setRadius(Radius);

    RenderTarget->draw(CircleShape);
}

////////////////////////////////////////////////////////////

void fi::Draw::circleOutline(sf::RenderTarget *RenderTarget, sf::Vector2f Position, float Radius, sf::Color Color, int Thickness)
{
    sf::CircleShape CircleShape;
    CircleShape.setOutlineColor(Color);
    CircleShape.setOutlineThickness(Thickness);
    CircleShape.setFillColor(sf::Color::Transparent);
    CircleShape.setPosition(Position);
    CircleShape.setOrigin(sf::Vector2f(Radius, Radius));
    CircleShape.setRadius(Radius);

    RenderTarget->draw(CircleShape);
}

////////////////////////////////////////////////////////////

void fi::Draw::circleOutline(sf::RenderTarget *RenderTarget, sf::Vector2f Position, float Radius, sf::Color Color, int Thickness, sf::Transform Transformation)
{
    sf::CircleShape CircleShape;
    CircleShape.setOutlineColor(Color);
    CircleShape.setOutlineThickness(Thickness);
    CircleShape.setFillColor(sf::Color::Transparent);
    CircleShape.setPosition(Position);
    CircleShape.setOrigin(sf::Vector2f(Radius, Radius));
    CircleShape.setRadius(Radius);

    RenderTarget->draw(CircleShape, Transformation);
}

////////////////////////////////////////////////////////////

void fi::Draw::circle(sf::RenderTarget *RenderTarget, sf::Vector2f Position, float Radius, sf::Color Color, sf::Transform Transformation)
{
    sf::CircleShape Test;
    Test.setFillColor(Color);
    Test.setPosition(Position);
    Test.setOrigin(sf::Vector2f(Radius, Radius));
    Test.setRadius(Radius);

    RenderTarget->draw(Test, Transformation);
}

////////////////////////////////////////////////////////////

void fi::Draw::circle(sf::RenderTarget *RenderTarget, sf::Vector2f Position, float Radius, sf::Color Color, int NumPoints)
{
    sf::CircleShape CircleShape;
    CircleShape.setFillColor(Color);
    CircleShape.setPosition(Position);
    CircleShape.setOrigin(sf::Vector2f(Radius, Radius));
    CircleShape.setRadius(Radius);
    CircleShape.setPointCount(NumPoints);
    RenderTarget->draw(CircleShape);
}

////////////////////////////////////////////////////////////

void fi::Draw::circle(sf::RenderTarget *RenderTarget, sf::Vector2f Position, float Radius, sf::Color Color, int NumPoints, sf::Transform Transformation)
{
    sf::CircleShape CircleShape;
    CircleShape.setFillColor(Color);
    CircleShape.setPosition(Position);
    CircleShape.setOrigin(sf::Vector2f(Radius, Radius));
    CircleShape.setRadius(Radius);
    CircleShape.setPointCount(NumPoints);

    RenderTarget->draw(CircleShape, Transformation);
}

////////////////////////////////////////////////////////////

void fi::Draw::circleOutline(sf::RenderTarget *RenderTarget, sf::Vector2f Position, float Radius, sf::Color Color, int NumPoints, int Thickness)
{
    sf::CircleShape CircleShape;
    CircleShape.setOutlineColor(Color);
    CircleShape.setOutlineThickness(Thickness);
    CircleShape.setFillColor(sf::Color::Transparent);
    CircleShape.setPosition(Position);
    CircleShape.setOrigin(sf::Vector2f(Radius, Radius));
    CircleShape.setRadius(Radius);
    CircleShape.setPointCount(NumPoints);

    RenderTarget->draw(CircleShape);
}

////////////////////////////////////////////////////////////

void fi::Draw::circleOutline(sf::RenderTarget *RenderTarget, sf::Vector2f Position, float Radius, sf::Color Color, int NumPoints, int Thickness, sf::Transform Transformation)
{
    sf::CircleShape CircleShape;
    CircleShape.setOutlineColor(Color);
    CircleShape.setOutlineThickness(Thickness);
    CircleShape.setFillColor(sf::Color::Transparent);
    CircleShape.setPosition(Position);
    CircleShape.setOrigin(sf::Vector2f(Radius, Radius));
    CircleShape.setRadius(Radius);
    CircleShape.setPointCount(NumPoints);

    RenderTarget->draw(CircleShape, Transformation);
}

////////////////////////////////////////////////////////////

void fi::Draw::triangle(sf::RenderTarget *RenderTarget, sf::Vector2i CenterPosition, int Direction, int Width, int Height, sf::Color Color)
{
    sf::Vertex triangle[4];
    triangle[0].color = Color;
    triangle[1].color = Color;
    triangle[2].color = Color;

    triangle[0].position = sf::Vector2f(CenterPosition.x, CenterPosition.y);
    triangle[1].position = triangle[0].position;
    triangle[2].position = triangle[0].position;

    float HalfWidth = Width / 2;
    float HalfHeight = Height / 2;

    if (Direction == LEFT)
    {
        triangle[0].position.x -= HalfWidth;

        triangle[1].position.x += HalfWidth;
        triangle[1].position.y += HalfHeight;

        triangle[2].position.x += HalfWidth;
        triangle[2].position.y -= HalfHeight;
    }
    if (Direction == RIGHT)
    {
        triangle[0].position.x += HalfWidth;

        triangle[1].position.x -= HalfWidth;
        triangle[1].position.y += HalfHeight;

        triangle[2].position.x -= HalfWidth;
        triangle[2].position.y -= HalfHeight;
    }
    else if (Direction == TOP)
    {
        triangle[0].position.y -= HalfHeight;

        triangle[1].position.x -= HalfWidth;
        triangle[1].position.y += HalfHeight;

        triangle[2].position.x += HalfWidth;
        triangle[2].position.y += HalfHeight;
    }
    else if (Direction == BOTTOM)
    {
        triangle[0].position.y += HalfHeight;

        triangle[1].position.x -= HalfWidth;
        triangle[1].position.y -= HalfHeight;

        triangle[2].position.x += HalfWidth;
        triangle[2].position.y -= HalfHeight;
    }

    triangle[3] = triangle[0];
    RenderTarget->draw(triangle, 4, sf::Quads);

    triangle[0].color = sf::Color::Black;
    triangle[1].color = sf::Color::Black;
    triangle[2].color = sf::Color::Black;
    triangle[3].color = sf::Color::Black;
    RenderTarget->draw(triangle, 4, sf::LineStrip);
}


////////////////////////////////////////////////////////////

void fi::Draw::appendLineToVertArray(sf::Vector2f a, sf::Vector2f b, sf::Color aColorToDraw, sf::Color bColorToDraw, float Thickness, sf::VertexArray &VertArray)
{
    sf::Vector2f Normal(a.y - b.y, b.x - a.x);
    float Length = std::sqrt(Normal.x * Normal.x + Normal.y * Normal.y);
    if (Length != 0.f)
        Normal /= Length;

    Normal = Normal * Thickness;
    Normal = Normal / 2.0f;

    sf::Vertex Vert[4] =
            {
                    sf::Vertex(sf::Vector2f(a) - Normal, aColorToDraw),
                    sf::Vertex(sf::Vector2f(b) - Normal, aColorToDraw),
                    sf::Vertex(sf::Vector2f(b) + Normal, bColorToDraw),
                    sf::Vertex(sf::Vector2f(a) + Normal, bColorToDraw)
            };

    VertArray.append(Vert[0]);
    VertArray.append(Vert[1]);
    VertArray.append(Vert[2]);
    VertArray.append(Vert[3]);
}

////////////////////////////////////////////////////////////

void fi::Draw::appendLineToVertArray(sf::Vector2f a, sf::Vector2f b, sf::Color ColorToDraw, float Thickness, sf::VertexArray &VertArray)
{
    sf::Vector2f Normal(a.y - b.y, b.x - a.x);
    float Length = std::sqrt(Normal.x * Normal.x + Normal.y * Normal.y);
    if (Length != 0.f)
        Normal /= Length;

    Normal = Normal * Thickness;
    Normal = Normal / 2.0f;

    sf::Vertex Vert[4] =
            {
                    sf::Vertex(a - Normal, ColorToDraw),
                    sf::Vertex(b - Normal, ColorToDraw),
                    sf::Vertex(b + Normal, ColorToDraw),
                    sf::Vertex(a + Normal, ColorToDraw)
            };

    VertArray.append(Vert[0]);
    VertArray.append(Vert[1]);
    VertArray.append(Vert[2]);
    VertArray.append(Vert[3]);
}


