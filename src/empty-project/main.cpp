#include "../common/engine.h"

namespace FI_Empty_App
{
    class Application : public fi::Application_Base
    {
    public:
        void setup() override {}
    };
}

int main()
{
    FI_Empty_App::Application Application;

    fi::getEngine().run(&Application);

    return 0;
}