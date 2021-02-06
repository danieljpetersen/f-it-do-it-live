#ifndef IUEINW_START_SCREEN_H
#define IUEINW_START_SCREEN_H

#include "../common/engine.h"

namespace IUEINW
{
    class IUEINW_Start_Screen : public fi::Plugin_Base
    {
    public:
        void onEnableChange(bool Enabling) override;
        void work(const int Event) override;

    private:
        bool Initialized = false;
        sf::Sprite BackgroundSprite;
    };
}

#endif
