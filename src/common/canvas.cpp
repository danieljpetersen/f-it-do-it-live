#include "canvas.h"
#include "grid.h"
#include "engine.h"

void fi::Canvas::create(int width, int height, sf::ContextSettings Settings)
{
    RenderTexture.create(width, height, Settings);
}

////////////////////////////////////////////////////////////

sf::RenderTarget *fi::Canvas::getRenderTarget()
{
    return &RenderTexture;
}

////////////////////////////////////////////////////////////

const sf::Texture *fi::Canvas::getTexture()
{
    return &RenderTexture.getTexture();
}

////////////////////////////////////////////////////////////

sf::Vector2f fi::Canvas::mapPixelToCoords(const sf::Vector2i &point, const sf::View &view) const
{
    return RenderTexture.mapPixelToCoords(point, view);
}

////////////////////////////////////////////////////////////

sf::Vector2f fi::Canvas::mapPixelToCoords(const sf::Vector2i &point) const
{
    return RenderTexture.mapPixelToCoords(point);
}

////////////////////////////////////////////////////////////

sf::Vector2i fi::Canvas::mapCoordsToPixel(const sf::Vector2f &point) const
{
    return RenderTexture.mapCoordsToPixel(point);
}

////////////////////////////////////////////////////////////

sf::Vector2i fi::Canvas::mapCoordsToPixel(const sf::Vector2f &point, const sf::View &view) const
{
    return RenderTexture.mapCoordsToPixel(point, view);
}

////////////////////////////////////////////////////////////

void fi::Canvas::display()
{
    MeshBuilder.drawToRenderTarget(&RenderTexture);

    RenderTexture.display();
}

////////////////////////////////////////////////////////////

void fi::Canvas::setSmooth(bool Value)
{
    RenderTexture.setSmooth(Value);
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder &fi::Canvas::draw()
{
    return MeshBuilder;
}

////////////////////////////////////////////////////////////

void fi::Canvas::draw(sf::VertexArray &VertexArray)
{
    RenderTexture.draw(VertexArray);
}

////////////////////////////////////////////////////////////

void fi::Canvas::draw(sf::VertexArray &VertexArray, sf::BlendMode BlendMode)
{
    RenderTexture.draw(VertexArray, BlendMode);
}

////////////////////////////////////////////////////////////

void fi::Canvas::draw(sf::VertexBuffer &VertexBuffer)
{
    RenderTexture.draw(VertexBuffer);
}

////////////////////////////////////////////////////////////

void fi::Canvas::draw(sf::VertexBuffer &VertexBuffer, sf::BlendMode BlendMode)
{
    RenderTexture.draw(VertexBuffer, BlendMode);
}

////////////////////////////////////////////////////////////

void fi::Canvas::clear()
{
    RenderTexture.clear();
}

////////////////////////////////////////////////////////////

void fi::Canvas::clear(sf::Color Color)
{
    RenderTexture.clear(Color);
}

////////////////////////////////////////////////////////////

void fi::Canvas::draw(sf::Text &T)
{
    RenderTexture.draw(T);
}

sf::Vector2u fi::Canvas::getSize()
{
    return RenderTexture.getSize();
}

////////////////////////////////////////////////////////////

void fi::Canvas::pan(int Direction, int PanSpeed)
{
    float Offset = PanSpeed * fi::getEngine().getDeltaTime().asMilliseconds();

    sf::Vector2f NewCenter = getCenter();
    if (Direction == LEFT)
    {
        NewCenter.x -= Offset;
        setCenter(NewCenter);
    }
    else if (Direction == RIGHT)
    {
        NewCenter.x += Offset;
        setCenter(NewCenter);

    }
    else if (Direction == TOP)
    {
        NewCenter.y -= Offset;
        setCenter(NewCenter);

    }
    else if (Direction == BOTTOM)
    {
        NewCenter.y += Offset;
        setCenter(NewCenter);
    }
}

////////////////////////////////////////////////////////////

void fi::Canvas::zoom(bool ZoomIn)
{
    if (ZoomIn)
    {
        zoom(CurrentZoomLevel - 1);
    }
    else
    {
        zoom(CurrentZoomLevel + 1);
    }
}

////////////////////////////////////////////////////////////

void fi::Canvas::setZoomLevels(int DefaultZoomLevel, int LowestZoomLevel, int HighestZoomLevel)
{
    this->CurrentZoomLevel = DefaultZoomLevel;
    this->DefaultZoomLevel = DefaultZoomLevel;
    this->LowestZoomLevel = LowestZoomLevel;
    this->HighestZoomLevel = HighestZoomLevel;
    this->NumberOfZoomLevels = std::abs(std::abs(HighestZoomLevel) + std::abs(LowestZoomLevel));
}

////////////////////////////////////////////////////////////

void fi::Canvas::zoom(int ZoomLevel)
{
    this->CurrentZoomLevel = ZoomLevel;

    sf::View View;
    View.reset(sf::FloatRect((float) 0, (float) 0, (float) RenderTexture.getSize().x, (float) RenderTexture.getSize().y));
    View.setCenter(RenderTexture.getView().getCenter());

    float CameraZoom = 1 + (CurrentZoomLevel * 0.1f);
    View.zoom(CameraZoom);

    RenderTexture.setView(View);
}

////////////////////////////////////////////////////////////

int fi::Canvas::getCurrentZoomLevel()
{
    return CurrentZoomLevel;
}

////////////////////////////////////////////////////////////

int fi::Canvas::getNumberOfZoomLevels()
{
    return NumberOfZoomLevels;
}

////////////////////////////////////////////////////////////

int fi::Canvas::getDefaultZoomLevel()
{
    return DefaultZoomLevel;
}

////////////////////////////////////////////////////////////

int fi::Canvas::getLowestZoomLevel()
{
    return LowestZoomLevel;
}

////////////////////////////////////////////////////////////

int fi::Canvas::getHighestZoomLevel()
{
    return HighestZoomLevel;
}

////////////////////////////////////////////////////////////

void fi::Canvas::resetZoom()
{
    zoom(DefaultZoomLevel);
}

////////////////////////////////////////////////////////////

void fi::Canvas::setCenter(sf::Vector2f Center)
{
    sf::View View = RenderTexture.getView();
    View.setCenter(Center.x, Center.y);
    RenderTexture.setView(View);
}

////////////////////////////////////////////////////////////

void fi::Canvas::setCenter(sf::Vector2i Center)
{
    sf::View View = RenderTexture.getView();
    View.setCenter(Center.x, Center.y);
    RenderTexture.setView(View);
}

////////////////////////////////////////////////////////////

sf::Vector2f fi::Canvas::getCenter()
{
    return RenderTexture.getView().getCenter();
}