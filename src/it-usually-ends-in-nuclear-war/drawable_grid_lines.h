#ifndef IUEINW_DRAWABLE_GRID_LINE_H
#define IUEINW_DRAWABLE_GRID_LINE_H

#include <SFML/Graphics.hpp>
#include "tile_types.h"
#include "../common/engine.h"

namespace IUEINW
{

    class IUEINW_Drawable_Grid_Lines_Around_Mouse : public fi::Plugin_Base
    {
    private:
        sf::VertexArray VertexArray;

        bool GridLinesAroundMouseEnabled;

    public:
        void onProgramStart() override;
        void onUpdate() override;
        void onDraw() override;
    };

    ////////////////////////////////////////////////////////////

    class IUEINW_Drawable_Grid_Lines_Always_On : public fi::Plugin_Base
    {
    private:
        bool GridLinesAlwaysOnNonVision, GridLinesAlwaysOnInVision;
        sf::VertexBuffer VBO;
        std::vector<sf::Vertex> Vertices;

    public:
        void onProgramStart() override;
        void onUpdate() override;
        void onDraw() override;
    };

    ////////////////////////////////////////////////////////////
    // note needs to come after drawablemap due to vision lines obscuring the border

    class IUEINW_Drawable_Map_Land_Ocean_Border : public fi::Plugin_Base
    {
    public:

        IUEINW_Drawable_Map_Land_Ocean_Border()
        {
            Lines.setPrimitiveType(sf::Lines);
        }

        sf::VertexArray Lines;
        void onUpdate() override;
        void onDraw() override { fi::getCanvasWorld().draw(Lines); } ;

    private:
        void addLandOceanBorderIfApplicable(int i);

    };
}

#endif