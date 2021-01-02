#ifndef IUEINW_DRAWABLE_GRID_LINE_H
#define IUEINW_DRAWABLE_GRID_LINE_H

#include <SFML/Graphics.hpp>
#include "tile_types.h"
#include "../common/engine.h"

namespace IUEINW
{
    class IUEINW_Drawable_Grid_Lines_Around_Mouse
    {
    private:
        sf::VertexArray VertexArray;

        bool GridLinesAroundMouseEnabled;

    public:
        void init();
        void buildDrawable();
        void draw();
    };

    ////////////////////////////////////////////////////////////

    class IUEINW_Drawable_Grid_Lines_Always_On
    {
    private:
        bool GridLinesAlwaysOnNonVision, GridLinesAlwaysOnInVision;
        sf::VertexBuffer VBO;
        std::vector<sf::Vertex> Vertices;

    public:
        void init();
        void buildDrawable();
        void draw();
    };

    ////////////////////////////////////////////////////////////
    // note needs to come after DrawableMap due to vision lines obscuring the border
    class IUEINW_Drawable_Map_Land_Ocean_Border
    {
    public:
        sf::VertexArray Lines;
        void init() { Lines.setPrimitiveType(sf::Lines); }
        void buildDrawable();
        void draw() { fi::getCanvasWorld().draw(Lines); } ;

    private:
        void addLandOceanBorderIfApplicable(int i);
    };
}

#endif