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
        void onProgramStart() ;
        void onUpdate() ;
        void onDraw() ;
    };

    ////////////////////////////////////////////////////////////

    class IUEINW_Drawable_Grid_Lines_Always_On
    {
    private:
        bool GridLinesAlwaysOnNonVision, GridLinesAlwaysOnInVision;
        sf::VertexBuffer VBO;
        std::vector<sf::Vertex> Vertices;

    public:
        void onProgramStart() ;
        void onUpdate() ;
        void onDraw() ;
    };

    ////////////////////////////////////////////////////////////
    // note needs to come after drawablemap due to vision lines obscuring the border

    class IUEINW_Drawable_Map_Land_Ocean_Border
    {
    public:
        sf::VertexArray Lines;
        void onProgramStart() { Lines.setPrimitiveType(sf::Lines); }
        void onUpdate() ;
        void onDraw()  { fi::getCanvasWorld().draw(Lines); } ;

    private:
        void addLandOceanBorderIfApplicable(int i);

    };
}

#endif