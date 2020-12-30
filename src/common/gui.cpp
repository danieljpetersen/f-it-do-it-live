#include "gui.h"
#include "engine.h"

void fi::GUI::init()
{
    auto FontPath = getOS().getPathToExecutableDirectory() + getConfig()["gui-style"]["font-path"].get<std::string>();

    if (!font.loadFromFile(FontPath))
    {
        getEngine().quitFromError("Failed to load font");
    }

}
