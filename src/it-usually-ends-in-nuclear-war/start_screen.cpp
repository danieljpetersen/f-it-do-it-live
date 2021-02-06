#include "start_screen.h"
#include "app.h"

void IUEINW::IUEINW_Start_Screen::onEnableChange(bool Enabling)
{
    if (Enabling)
    {
        if (! Initialized)
        {
            Initialized = true;

            auto optional = fi::getEngine().TextureStore.addTexture(fi::getOS().getPathToExecutableDirectory() + "images/background-menu.png");

            if (optional.has_value())
            {
                BackgroundSprite.setTexture(*optional.value());
            }
        }
    }
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Start_Screen::work(const int Event)
{
    fi::getWindow().draw(BackgroundSprite);

    //int ImageArea = fi::guiRow(0.4f);
    //fi::guiImage(fi::getGUI().Nodes[ImageArea], fi::getOS().getPathToExecutableDirectory() + "images/logo.png");

    //int HandledIndex = -1;
    //std::vector<std::string> ButtonLabels = { "Start", "Load Game", "Options", "Hall of Fame", "Join The Discord", "Credits", "Exit To Desktop" };
    //fi::guiButtonsVertical(new fi::GUI_Node(fi::ANCHOR_CENTER_ABSOLUTE, fi::ANCHOR_START, sf::Vector2f(240, 30)), ButtonLabels, HandledIndex);

    //if (fi::getInput().check(fi::key::Mouse1))
    {
        getApp().setProgramStateCoreScreen();
    }
}
