#ifndef IT_USUALLY_ENDS_IN_NUCLEAR_WAR_DRAWABLE_MAP_H
#define IT_USUALLY_ENDS_IN_NUCLEAR_WAR_DRAWABLE_MAP_H

#include "../common/engine.h"
#include "drawable_grid_lines.h"
#include "drawable_map_edges.h"

namespace IUEINW
{
    class IUEINW_Map_Drawables
    {
    public:
        IUEINW_Drawable_Grid_Lines_Around_Mouse DrawableGridLinesAroundMouse;
        IUEINW_Drawable_Grid_Lines_Always_On DrawableGridLinesAlwaysOn;
        IUEINW_Drawable_Map_Edges DrawableMapEdges;
        IUEINW_Drawable_Map_Land_Ocean_Border DrawableMapLandOceanBorder;

        void draw();
        void buildMapDrawable();
        void setTileVertexColors(int TileIndex, bool ExploredTile);
        void setTileVertexColors(int TileIndex);
        void setTileVertexColors(int TileIndex, sf::Color Color); // for debugging
        void setTileVertexColors();

        bool ShowDebugProvinceTiles = false;
        std::vector<sf::VertexBuffer> VBO;
        std::vector<std::vector<sf::Vertex>> Vertices;
    };

    IUEINW_Map_Drawables &getMapDrawables();

    // ----

    class IUEINW_Plugin_Init_Map_Drawables : public fi::Plugin_Base
    {
    public:
        void work(const int Event) override;
    };

    // ----

    class IUEINW_Plugin_Build_Map_Drawables : public fi::Plugin_Base
    {
    public:
        void work(const int Event) override;
    };

    // ----

    class IUEINW_Plugin_Draw_Map : public fi::Plugin_Base
    {
    public:
        void work(const int Event) override;
    };
}
#endif