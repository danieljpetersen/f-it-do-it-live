#include "canvas.h"
#include "grid.h"
#include "engine.h"

void fi::Canvas::create(int width, int height, bool KeepCurrentCenter, sf::ContextSettings Settings)
{
    auto Center = RenderTexture.getView().getCenter();
    RenderTexture.create(width, height, Settings);

    if (KeepCurrentCenter)
	{
		sf::View v = RenderTexture.getView();
		v.reset(sf::FloatRect((float)0, (float)0, width, height));
		v.setCenter((Center));
		RenderTexture.setView(v);
	}
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

sf::View fi::Canvas::getView()
{
    return RenderTexture.getView();
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
	MeshBuilder.clear();
    RenderTexture.clear();
}

////////////////////////////////////////////////////////////

void fi::Canvas::clear(sf::Color Color)
{
	MeshBuilder.clear();
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
    if (ZoomLevel < this->LowestZoomLevel)
    {
        ZoomLevel = LowestZoomLevel;
    }
    else if (ZoomLevel > this->HighestZoomLevel)
    {
        ZoomLevel = HighestZoomLevel;
    }

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

////////////////////////////////////////////////////////////

bool fi::Canvas::handleCameraInput(bool Pan, bool Zoom, bool CenterOnMouseClick, bool MouseDrag, bool MouseWindowEdgeScrolling)
{
    bool Handled = false;
    if (Pan)
    {
        const int KeyboardPanSpeed = 2;
        if (fi::getInput().check("Camera Pan Up"))
        {
            Handled = true;
            fi::getCanvasWorld().pan(TOP, KeyboardPanSpeed);
        }
        if (fi::getInput().check("Camera Pan Down"))
        {
            Handled = true;
            fi::getCanvasWorld().pan(BOTTOM, KeyboardPanSpeed);
        }
        if (fi::getInput().check("Camera Pan Left"))
        {
            Handled = true;
            fi::getCanvasWorld().pan(LEFT, KeyboardPanSpeed);
        }
        if (fi::getInput().check("Camera Pan Right"))
        {
            Handled = true;
            fi::getCanvasWorld().pan(RIGHT, KeyboardPanSpeed);
        }
    }

    if (Zoom)
    {
        if (fi::getInput().check("Camera Zoom In"))
        {
            Handled = true;
            fi::getCanvasWorld().zoom(true);
        }
        if (fi::getInput().check("Camera Zoom Out"))
        {
            Handled = true;
            fi::getCanvasWorld().zoom(false);
        }

        if (fi::getInput().check("Camera Zoom Reset"))
        {
            Handled = true;
            fi::getCanvasWorld().resetZoom();
        }
    }

    if (CenterOnMouseClick)
    {
        if (fi::getInput().check("Camera Center On Mouse"))
        {
            Handled = true;
            bool PerformCentering = true;
            sf::Int32 TimeOfLastInitialDown = fi::getClock().getElapsedTime().asMilliseconds() - fi::getInput().timeOfLastInitialDown("Camera Center On Mouse").asMilliseconds();

            int xChange = fi::getMouseWindowPosition2i().x - fi::getInput().MouseWindowPositionOnLastMouseDown.x;
            int yChange = fi::getMouseWindowPosition2i().y - fi::getInput().MouseWindowPositionOnLastMouseDown.y;
            int TotalChange = abs(xChange) + abs(yChange);
            if (TotalChange > 10)
            {
                PerformCentering = false;
            }
            else if (TimeOfLastInitialDown > 200) // because if user holds down left click should cancel it out after n time
            {
                PerformCentering = false;
            }

            if (PerformCentering)
            {
                fi::getCanvasWorld().setCenter(fi::getInput().MouseWorldPosition);
            }

        }
    }

    if (MouseDrag)
    {
        if (fi::getInput().check("Camera Mouse Drag"))
        {
            Handled = true;
            sf::Vector2i LMD = fi::getInput().MouseWindowPositionOnLastLoop;
            sf::Vector2i NOW = fi::getMouseWindowPosition2i();
            sf::Vector2f LMD_World = fi::getCanvasWorld().mapPixelToCoords(LMD);
            sf::Vector2f NOW_World = fi::getCanvasWorld().mapPixelToCoords(NOW);
            float xDiff = float(LMD_World.x - NOW_World.x);
            float yDiff = float(LMD_World.y - NOW_World.y);
            sf::Vector2f NewCenter = fi::getCanvasWorld().getCenter();
            NewCenter.x += xDiff;
            NewCenter.y += yDiff;
            fi::getCanvasWorld().setCenter(NewCenter);
        }
    }

    if (MouseWindowEdgeScrolling)
    {
        bool MousePanOnFullscreen = fi::getConfig()["controls"]["mouse-pan-on-fullscreen"].get<bool>(); // todo save to member variable
        bool MousePanOnWindowed = fi::getConfig()["controls"]["mouse-pan-on-windowed"].get<bool>(); // todo save to member variable

        bool AttemptMouseWindowEdgeScrolling = false;
        if ((fi::getEngine().isFullscreen() != true) && (MousePanOnWindowed))
        {
            AttemptMouseWindowEdgeScrolling = true;
        }
        else if ((fi::getEngine().isFullscreen()) && (MousePanOnFullscreen))
        {
            AttemptMouseWindowEdgeScrolling = true;
        }

        if (AttemptMouseWindowEdgeScrolling)
        {
            if (fi::getClock().getElapsedTime().asMilliseconds() > fi::getInput().timeOfLastInitialDown("Camera Mouse Drag").asMilliseconds() + 400)
            {
                const int EDGE_SCROLL_SIZE = 2;
                const int MouseWindowEdgePanSpeed = 2;
                // ---- x
                {
                    if (fi::getMouseWindowPosition2f().x < EDGE_SCROLL_SIZE)
                    {
                        Handled = true;
                        fi::getCanvasWorld().pan(LEFT, MouseWindowEdgePanSpeed);
                    }
                    else if (fi::getMouseWindowPosition2f().x > fi::getWindow().getSize().x - EDGE_SCROLL_SIZE)
                    {
                        Handled = true;
                        fi::getCanvasWorld().pan(RIGHT, MouseWindowEdgePanSpeed);
                    }
                }

                // ---- y
                {
                    if (fi::getMouseWindowPosition2f().y < EDGE_SCROLL_SIZE)
                    {
                        Handled = true;
                        fi::getCanvasWorld().pan(TOP, MouseWindowEdgePanSpeed);
                    }
                    else if (fi::getMouseWindowPosition2f().y > fi::getWindow().getSize().y - EDGE_SCROLL_SIZE)
                    {
                        Handled = true;
                        fi::getCanvasWorld().pan(BOTTOM, MouseWindowEdgePanSpeed);
                    }
                }
            }
        }
    }

    return Handled;
}
