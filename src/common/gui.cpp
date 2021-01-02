#include "gui.h"
#include "engine.h"

void fi::GUI::init()
{
    auto FontPath = getOS().getPathToExecutableDirectory() + getConfig()["gui-style"]["font-path"].get<std::string>();

    if (!font.loadFromFile(FontPath))
    {
        getEngine().quitFromError("Failed to load font");
    }

    ImGui::GetIO().Fonts->Clear();
    auto ImGuiFontSize = getConfig()["gui-style"]["imgui-font-size"].get<float>();
    ImGui::GetIO().Fonts->AddFontFromFileTTF(FontPath.c_str(), ImGuiFontSize);

    ImGui::SFML::UpdateFontTexture(); // important call: updates font texture

}
