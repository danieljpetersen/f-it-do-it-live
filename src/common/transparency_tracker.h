#ifndef FI_TRANSPARENCY_TRACKER_H
#define FI_TRANSPARENCY_TRACKER_H

#include <SFML/System.hpp>

namespace fi
{
    class Transparency_Tracker
    {
    public:
        sf::Time TimeAdded;
        sf::Uint8 CurrentTransparency;
    };

    // ----

    class Tile_Transparency_Tracker : public fi::Transparency_Tracker
	{
	public:
        int AssociatedTile;
    };
}

////////////////////////////////////////////////////////////

#endif