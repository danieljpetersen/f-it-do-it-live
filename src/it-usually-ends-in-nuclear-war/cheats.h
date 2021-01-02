#ifndef IUEINW_CHEATS_H
#define IUEINW_CHEATS_H

#include "../common/plugins.h"

namespace IUEINW
{
    class IUEINW_Plugin_Init_Cheats : public fi::Plugin_Base
    {
    public:
        void work(const int Event) override;
    };

    // ----

    class IUEINW_Cheats
	{
	private:
		bool RevealAllTerrain, NoFogOfWar;
		bool Active;

	public:
		void reset();

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