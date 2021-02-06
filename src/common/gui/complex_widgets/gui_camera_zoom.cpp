#include <SFML/Graphics/RenderTarget.hpp>
#include <stdlib.h>
#include "gui_camera_zoom.h"
#include "../../engine.h"

void fi::Plugin_Camera_Zoom_GUI::work(const int EventID)
{
	int ZoomLevel = getCanvasWorld().getCurrentZoomLevel();
	if (ZoomLevel != LastZoomLevel)
	{
		LastZoomLevel = ZoomLevel;

		Transparency.CurrentTransparency = 255;
		Transparency.TimeAdded = fi::getClock().getElapsedTime();

    }

	// ----

    if (Transparency.CurrentTransparency > 0)
    {
        const int NumberOfNotches = fi::getCanvasWorld().getNumberOfZoomLevels() + 1;
        const int LowestAbsolute = abs(fi::getCanvasWorld().getLowestZoomLevel());

        sf::RenderTarget *RenderTarget = fi::getCanvasGUI().getRenderTarget();

        sf::VertexArray Quad;
        Quad.setPrimitiveType(sf::Lines);

        sf::Color Color = sf::Color(255, 255, 255, Transparency.CurrentTransparency);
        sf::Vertex line[2];

        int TotalLineHeight = 300;
        int _HalfNotchWidth = 6;
        int HalfNotchWidth = _HalfNotchWidth;
        int BGWidth = _HalfNotchWidth * 4 + fi::getGUI().SPACING_FROM_WIDGET_EDGE;
        int BGHeight = TotalLineHeight + fi::getGUI().SPACING_FROM_WIDGET_EDGE * 2;

        int HalfLineHeight = TotalLineHeight / 2;
        int yLineStart = fi::getWindow().getSize().y / 2 - HalfLineHeight;
        int yLineEnd = fi::getWindow().getSize().y / 2 + HalfLineHeight;
        int xPosition = fi::getWindow().getSize().x - fi::getGUI().SPACING_FROM_SCREEN_EDGE - BGWidth / 2;

        float SpacingBetweenNotches = (float) TotalLineHeight / float(NumberOfNotches - 1);
        float yPosition = yLineStart;

        int xLeft = xPosition - (_HalfNotchWidth * 2) - fi::getGUI().SPACING_FROM_WIDGET_EDGE / 2;
        int yLeft = yLineStart - fi::getGUI().SPACING_FROM_WIDGET_EDGE;

        // ---- create Notches
        {
            for (int i = 0; i < NumberOfNotches; i++)
            {
                // current zoom indicator
                if (i == ZoomLevel + LowestAbsolute)
                {
                    Color.r = 255;
                    Color.g = 0;
                    Color.b = 0;
                    HalfNotchWidth = _HalfNotchWidth * 2;
                }
                else if (i == LowestAbsolute)
                {
                    HalfNotchWidth = _HalfNotchWidth * 2;
                    //Color.r = 100;
                    //Color.g = 215;
                    //Color.b = 210;
                    Color = fi::getGUI().getStyle()->BGSelected;
                }
                else
                {
                    Color.r = 255;
                    Color.g = 255;
                    Color.b = 255;
                    HalfNotchWidth = _HalfNotchWidth;
                }
                Color.a = Transparency.CurrentTransparency;

                line[0] = sf::Vertex(sf::Vector2f(xPosition - HalfNotchWidth, yPosition), Color);
                line[1] = sf::Vertex(sf::Vector2f(xPosition + HalfNotchWidth, yPosition), Color);

                Quad.append(line[0]);
                Quad.append(line[1]);

                yPosition += SpacingBetweenNotches;
            }
        }

        // ---- create Vertical Line
        {
            Color.r = 255;
            Color.g = 255;
            Color.b = 255;
            line[0] = sf::Vertex(sf::Vector2f(xPosition, yLineStart), Color);
            line[1] = sf::Vertex(sf::Vector2f(xPosition, yLineEnd), Color);
            Quad.append(line[0]);
            Quad.append(line[1]);
        }

        // ---- draw BG
        {
            sf::Color BG = fi::getGUI().getStyle()->BGRegular;
            sf::Color HL = fi::getGUI().getStyle()->BorderRegular;

            float Normalized = Transparency.CurrentTransparency / 255.f;
            BG.a = sf::Uint8(BG.a * Normalized);
            HL.a = sf::Uint8(HL.a * Normalized);

            fi::Draw::rect(RenderTarget, sf::Vector2i(xLeft, yLeft), BGWidth, BGHeight, BG);
            fi::Draw::rectOutline(RenderTarget, sf::Vector2i(xLeft, yLeft), BGWidth, BGHeight, HL);
        }

        // ---- Draw
        {
            RenderTarget->draw(Quad);
        }

        // ---- Update Transparency
        {
            sf::Int32 Diff = (fi::getClock().getElapsedTime().asMilliseconds() - Transparency.TimeAdded.asMilliseconds()) / 8;
            sf::Int32 Value = (255 - Diff);
            if (Value <= 0)
                Value = 0;
            Transparency.CurrentTransparency = (sf::Uint8)Value;
         }
    }
}