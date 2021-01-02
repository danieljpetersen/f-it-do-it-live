#ifndef IT_USUALLY_ENDS_IN_NUCLEAR_WAR_DRAWABLE_FRONT_DIRT_H
#define IT_USUALLY_ENDS_IN_NUCLEAR_WAR_DRAWABLE_FRONT_DIRT_H

#include "../common/engine.h"

namespace IUEINW
{
    class IUEINW_Drawable_Map_Edges
    {
    private:
        sf::VertexArray Quads;
        int FrontDirtHeight;

    public:
        void onProgramStart() ;
        void onUpdate() ;
        void onDraw() ;
    };
}
#endif