#ifndef FI_TRANSPARENCY_TRACKER_H
#define FI_TRANSPARENCY_TRACKER_H

#include <SFML/System.hpp>

namespace fi
{
	class Tile_Transparency_Tracker
	{
	public:
		sf::Time TimeAdded;
		sf::Uint8 CurrentTransparency;
        int AssociatedTile;
    };
}

////////////////////////////////////////////////////////////


#endif