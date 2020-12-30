#ifndef GR_TRANSPARENCY_TRACKER_H
#define GR_TRANSPARENCY_TRACKER_H

#include <SFML/System.hpp>

namespace IUEINW
{
	class IUEINW_Transparency_Tracker
	{
	public:
		sf::Time TimeAdded;
		sf::Uint8 CurrentTransparency;
	};
}

////////////////////////////////////////////////////////////


#endif