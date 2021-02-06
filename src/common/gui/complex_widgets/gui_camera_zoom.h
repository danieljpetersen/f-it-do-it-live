#ifndef FI_CAMERA_ZOOM_DRAWABLE_H
#define FI_CAMERA_ZOOM_DRAWABLE_H

#include "../../plugins.h"
#include "../../transparency_tracker.h"

namespace fi
{
	class Plugin_Camera_Zoom_GUI : public fi::Plugin_Base
	{
	private:
		int LastZoomLevel = 0;
		fi::Transparency_Tracker Transparency;

	public:
		void work(const int EventID) override;
	};
}

#endif