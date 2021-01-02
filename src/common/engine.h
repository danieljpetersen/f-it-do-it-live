#ifndef FI_FRAMEWORK_H
#define FI_FRAMEWORK_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include "nlohmann_json.h"
#include "nameof.h"
#include "non_copyable.h"
#include "non_movable.h"
#include "singleton.h"
#include "util.h"
#include "os.h"
#include "color.h"
#include "fi_math.h"
#include "random.h"
#include "cursor.h"
#include "input.h"
#include "audio.h"
#include "plugins.h"
#include "canvas.h"
#include "frame_clock.h"
#include "gui.h"
#include "tick.h"
#include "grid.h"
#include "slot_map.h"
#include "loading_plugins.h"

namespace fi
{
    class Application_Base
    {
    public:
        virtual void setup() = 0;
    };

    ////////////////////////////////////////////////////////////

    class Engine : public Singleton<Engine>
    {
    public:
        void run(Application_Base *App);
        void quitFromError(std::string err);

        // note -- user can bypass some functions via calling directly to Engine.Window,  ie:  setCursorVisible
        // if bypassed, it results in bug where settings won't persist in situations like a fullscreen toggle.
        // not sure how to fix this other than making a wrapper around sf::Window, which I do not want to do.
        void setCursorStyle(const sf::StandardCursor::TYPE Type);
        void setCursorGrabbed(bool Value);
        void setCursorVisible(bool Value);
        bool setWindowIcon(std::string FullPathToIcon);
        void setBackgroundColor(sf::Color Color);
        void setBackgroundColor(sf::Uint8 r, sf::Uint8 g, sf::Uint8 b);
        void toggleFullscreen();

        bool isFullscreen();
        sf::Uint8 getWindowStyle();
        bool getCursorGrabbed();
        bool getCursorVisible();
        sf::Time getDeltaTime();
        bool isSuspended();
        sf::Uint64 getFrameCount();
        fi::Tick *getTick(std::string TickID);
        fi::Tick *getTick(int TickID);

        sf::Clock Clock;
        sf::RenderWindow Window;
        nlohmann::json jsonConfig;
        fi::Operating_System OS;
        fi::Random_Generator RNG;
        fi::Input Input;
        fi::Audio Audio;
        fi::Plugin_Manager Plugins;
        std::vector<fi::Tick> Ticks;
        fi::GUI GUI;
        fi::Canvas CanvasWorld;
        fi::Canvas CanvasGUI;

    private:
        Application_Base *App;

        void initJSON();
        void initWindow();
        void initWindow(unsigned int AALevel, std::string WindowTitle, sf::Uint32 WindowStyle, int xPosition, int yPosition, int Width, int Height, bool CursorGrabbed, int FrameLimit, bool SuspendOnLostFocus);
        void initCanvases();
        void initTicks();
        void mainThreadLoop();
        void handleDefaultInput();
        void executeResize(float x, float y);
        void saveJSONConfig();
        void stepTicks();
        void updateWindowTitleIfApplicable();
        void drawCanvasesToScreen();

        bool QuitFlag = false;
        bool SuspendOnLostFocus = false;
        bool CurrentlySuspended = false;
        bool UnsuspendThisFrame = false;

        bool CursorGrabbed = false;
        bool CursorVisible = true;
        sf::StandardCursor Cursors[4];
        int	ActiveCursorIndex = -1;

        Clock_HUD ClockHUD;
        sf::Color BackgroundColor;
        sf::Time DeltaTime;

        std::unordered_map<std::string, int> TickIdToIndexMap;
        std::unordered_map<int, std::string> TickIndexToIdMap;

        friend class Input;
        friend class Program_State_Builder;

    };

    ////////////////////////////////////////////////////////////

    Engine &getEngine();
    Operating_System &getOS();
    nlohmann::json &getConfig();
    fi::Input &getInput();
    sf::Time getDeltaTime();
    sf::Clock &getClock();
    fi::Audio getAudio();
    fi::Tick *getTick(std::string TickID);
    fi::Tick *getTick(int TickID);
    fi::Plugin_Manager &getPlugins();
    fi::Canvas &getCanvasWorld();
    fi::Canvas &getCanvasGUI();
    sf::RenderWindow &getWindow();
}

#endif