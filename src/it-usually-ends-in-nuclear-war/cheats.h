#ifndef IUEINW_CHEATS_H
#define IUEINW_CHEATS_H

#include "../common/plugins.h"

namespace IUEINW
{
    // convert to ecs
    // use events to transform data

    struct Enabled_Cheats
    {

    };

	class IUEINW_Cheats : public fi::Plugin_Base
	{
	private:
		bool RevealAllTerrain, NoFogOfWar;
		bool Active;

	public:
		void onProgramStart() override;

		void toggleFow();
		void toggleRevealTerrain();
		void setRevealTerrain(bool Value);
		void setNoFogOfWar(bool Value);
		bool isRevealAllTerrainEnabled();
		bool isNoFogOfWarEnabled();

		void activate();
		void deactivate();
		bool isActive();
	};

	IUEINW_Cheats &getCheats();
}

#endif