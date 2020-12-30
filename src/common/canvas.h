#ifndef FI_CANVAS_H
#define FI_CANVAS_H

#include <SFML/Graphics.hpp>
#include "mesh_builder.h"

namespace fi
{
    class Canvas
    {
    public:
        void create(int width, int height, sf::ContextSettings Settings);

        sf::RenderTarget *getRenderTarget();
        const sf::Texture *getTexture();

        sf::Vector2f mapPixelToCoords(const sf::Vector2i& point, const sf::View& view) const;
        sf::Vector2f mapPixelToCoords(const sf::Vector2i& point) const;
        sf::Vector2i mapCoordsToPixel(const sf::Vector2f& point) const;
        sf::Vector2i mapCoordsToPixel(const sf::Vector2f& point, const sf::View& view) const;
        sf::Vector2u getSize();

        Mesh_Builder &draw();
        void draw(sf::Text &T);
        void draw(sf::VertexArray &VertexArray);
        void draw(sf::VertexBuffer &VertexBuffer);
        void draw(sf::VertexArray &VertexArray, sf::BlendMode BlendMode);
        void draw(sf::VertexBuffer &VertexBuffer, sf::BlendMode BlendMode);

        void clear();
        void clear(sf::Color Color);
        void setSmooth(bool Value);
        void display();

        void pan(int Direction, int PanSpeed);
        void zoom(bool ZoomIn);
        void zoom(int ZoomLevel);
        void setZoomLevels(int DefaultZoomLevel, int LowestZoomLevel, int HighestZoomLevel);
        int getCurrentZoomLevel();
        int getNumberOfZoomLevels();
        int getDefaultZoomLevel();
        int getLowestZoomLevel();
        int getHighestZoomLevel();
        void resetZoom();

        void setCenter(sf::Vector2f Center);
        void setCenter(sf::Vector2i Center);
        sf::Vector2f getCenter();

    private:
        sf::RenderTexture RenderTexture;
        Mesh_Builder MeshBuilder;

        int CurrentZoomLevel = 0;
        int DefaultZoomLevel = 0;
        int LowestZoomLevel = 0;
        int HighestZoomLevel = 0;
        int NumberOfZoomLevels = 0;
    };
}

#endif