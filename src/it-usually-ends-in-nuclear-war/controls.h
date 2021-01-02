#ifndef FI_CONTROLS_H
#define FI_CONTROLS_H

#include "../common/plugins.h"

namespace IUEINW
{
    class IUEINW_Plugin_Controls : public fi::Plugin_Base
    {
    public:
        void work(const int Event) override;
    };
}

#endif