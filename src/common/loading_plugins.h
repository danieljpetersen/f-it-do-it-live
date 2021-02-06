#ifndef FI_LOADING_PLUGINS_H
#define FI_LOADING_PLUGINS_H

#include "random.h"
#include "cursor.h"

namespace fi
{
	class Plugin_Loading_Start : public fi::Plugin_Base
	{
	public:
		void work(const int Event) override
		{
		    fi::getRandom().seed();
            fi::setCursorStyle(sf::StandardCursor::WAIT);
            fi::Double_Buffer_Object_Tracker::instance().reset();
            getCoreTick()->setTickCount(0);
            getCoreTick()->pauseTimestep();
        }
	};

	// ----

	class Plugin_Loading_End : public fi::Plugin_Base
	{
	public:
		void work(const int Event) override
		{
            fi::Double_Buffer_Object_Tracker::instance()._swapPointers();
		    fi::setCursorStyle(sf::StandardCursor::NORMAL);
		}
	};
}

#endif