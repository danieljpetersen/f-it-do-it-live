#ifndef FI_DRAW_H
#define FI_DRAW_H

#include <vector>
#include <SFML/Graphics.hpp>

namespace fi
{
    // this is all fucked and needs to be burned with fire. No real rhyme or reason to anything added to on an as needed basis

    namespace Draw
    {
        void appendLineToVertArray(sf::Vector2f a, sf::Vector2f b, sf::Color ColorToDraw, float Thickness, sf::VertexArray &VertArray);
        void appendLineToVertArray(sf::Vector2f a, sf::Vector2f b, sf::Color aColorToDraw, sf::Color bColorToDraw, float Thickness, sf::VertexArray &VertArray);

        void text(std::string Str, int x, int y);
        void text(sf::RenderTarget *RenderTarget, std::string Str, int x, int y);

        void rect(sf::VertexArray &vertexArray, sf::Vector2f Position, int Width, int Height, sf::Color ColorToDraw);
        void rect(sf::VertexArray &vertexArray, sf::Vector2i Position, int Width, int Height, sf::Color ColorToDraw);
        void rect(std::vector<sf::Vertex> &vertexBuffer, sf::Vector2f Position, int Width, int Height, sf::Color ColorToDraw);
        void rect(std::vector<sf::Vertex> &vertexBuffer, sf::Vector2i Position, int Width, int Height, sf::Color ColorToDraw);

        void rect(sf::RenderTarget *RenderTarget, sf::Vector2f Position, int Width, int Height, sf::Color ColorToDraw);
        void rect(sf::RenderTarget *RenderTarget, sf::Vector2i Position, int Width, int Height, sf::Color ColorToDraw);
        void rect(sf::RenderTarget *RenderTarget, sf::Vector2i Position, int Width, int Height, sf::Color ColorToDraw, sf::Transform Transformation);

        void rectOutline(sf::VertexArray &VertexArray, sf::Vector2i Position, int Width, int Height, sf::Color ColorToDraw, sf::Transform Transformation);
        void rectOutline(sf::VertexArray &VertexArray, sf::Vector2i Position, int Width, int Height, sf::Color ColorToDraw);
        void rectOutline(sf::VertexArray &VertexArray, sf::Vector2f Position, int Width, int Height, sf::Color ColorToDraw);
        void rectOutline(sf::RenderTarget *RenderTarget, sf::Vector2i Position, int Width, int Height, sf::Color ColorToDraw, sf::Transform Transformation);
        void rectOutline(sf::RenderTarget *RenderTarget, sf::Vector2i Position, int Width, int Height, sf::Color ColorToDraw);

        void line(std::vector<sf::Vertex> &vertexBuffer, sf::Vector2f a, sf::Vector2f b, sf::Color ColorToDraw);
        void line(std::vector<sf::Vertex> &vertexBuffer, sf::Vector2f a, sf::Vector2f b, sf::Color ColorToDraw, float Thickness);
        void line(sf::VertexArray &vertexArray, sf::Vector2f a, sf::Vector2f b, sf::Color ColorToDraw);
        void line(sf::VertexArray &vertexArray, sf::Vector2f a, sf::Vector2f b, sf::Color ColorToDraw, float Thickness);
        void line(sf::RenderTarget *RenderTarget, sf::Vector2f a, sf::Vector2f b, sf::Color ColorToDraw, float Thickness);
        void line(sf::RenderTarget *RenderTarget, sf::Vector2i a, sf::Vector2i b, sf::Color ColorToDraw);
        void line(sf::RenderTarget *RenderTarget, sf::Vector2f a, sf::Vector2f b, sf::Color ColorToDraw);
        void line(sf::RenderTarget *RenderTarget, sf::Vector2i a, sf::Vector2i b, sf::Color ColorToDraw, sf::Transform Transformation);
        void line(sf::RenderTarget *RenderTarget, sf::Vector2i a, sf::Vector2i b, sf::Color ColorToDraw, float Thickness);
        void line(sf::RenderTarget *RenderTarget, sf::Vector2i a, sf::Vector2i b, sf::Color ColorToDraw, float Thickness, sf::Transform Transformation);
        void line(sf::RenderTarget *RenderTarget, sf::Vector2i a, sf::Vector2i b, sf::Color aColorToDraw, sf::Color bColorToDraw, float Thickness);
        void line(sf::VertexArray &VertexArray, sf::Vector2i a, sf::Vector2i b, sf::Color ColorToDraw, float Thickness);
        void line(sf::VertexArray &VertexArray, sf::Vector2i a, sf::Vector2i b, sf::Color aColorToDraw, sf::Color bColorToDraw, float Thickness);

        void circle(std::vector<sf::Vertex> *VBO, int VBOStartIndex, int NumPoints, sf::Vector2f Position, float Radius, sf::Color Color);
        void circle(sf::RenderTarget *RenderTarget, sf::Vector2f Position, float Radius, sf::Color Color);
        void circle(sf::RenderTarget *RenderTarget, sf::Vector2f Position, float Radius, sf::Color Color, sf::Transform Transformation);
        void circle(sf::RenderTarget *RenderTarget, sf::Vector2f Position, float Radius, sf::Color Color, int NumPoints);
        void circle(sf::RenderTarget *RenderTarget, sf::Vector2f Position, float Radius, sf::Color Color, int NumPoints, sf::Transform Transformation);
        void circleOutline(sf::RenderTarget *RenderTarget, sf::Vector2f Position, float Radius, sf::Color Color, int NumPoints, int Thickness);
        void circleOutline(sf::RenderTarget *RenderTarget, sf::Vector2f Position, float Radius, sf::Color Color, int NumPoints, int Thickness, sf::Transform Transformation);
        void circleOutline(sf::RenderTarget *RenderTarget, sf::Vector2f Position, float Radius, sf::Color Color, int Thickness);
        void circleOutline(sf::RenderTarget *RenderTarget, sf::Vector2f Position, float Radius, sf::Color Color, int Thickness, sf::Transform Transformation);

        void triangle(sf::RenderTarget *RenderTarget, sf::Vector2i CenterPosition, int Direction, int Width, int Height, sf::Color Color);
    }
}

#endif