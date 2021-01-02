#include <utility>
#include "engine.h"

////////////////////////////////////////////////////////////

void fi::Engine::run(fi::Application_Base *App)
{
    this->App = App;

    initJSON();
    initTicks();
    Input.init();
    Audio.init();
    initWindow();
    initCanvases();
    GUI.init();
    ClockHUD.init(&GUI.font);

    App->setup();

    Plugins.changeStateIfApplicable();

    executeResize(Window.getSize().x, Window.getSize().y);

    mainThreadLoop();

    saveJSONConfig();
}

////////////////////////////////////////////////////////////

void fi::Engine::quitFromError(std::string err)
{
    log(std::move(err));

    Window.close();

    std::abort();
}

////////////////////////////////////////////////////////////

void fi::Engine::mainThreadLoop()
{
    sf::Clock DeltaTimeClock;

    while (!QuitFlag)
    {
        ClockHUD.beginFrame();

        QuitFlag = Input.record();

        handleDefaultInput();

        if (QuitFlag)
        {
            Window.close();
        }
        else
        {
            if (CurrentlySuspended)
            {
                Audio.muteSounds();

                sf::sleep(sf::milliseconds(20));

                ClockHUD.endFrame();

                continue;
            }
            else if (UnsuspendThisFrame)
            {
                Audio.unmuteSounds();

                UnsuspendThisFrame = false;
            }
            updateWindowTitleIfApplicable();

            Window.clear(sf::Color::Black);
            CanvasWorld.clear(BackgroundColor);
            CanvasGUI.clear(sf::Color::Transparent);

            ImGui::SFML::Update(Window, getDeltaTime());

            stepTicks();
            Plugins.changeStateIfApplicable();
            Plugins.execute(EVENT_PRE_UPDATE);
            Plugins.execute(EVENT_UPDATE);
            Plugins.execute(EVENT_POST_UPDATE);
            Plugins.execute(EVENT_DRAW);

            ClockHUD.drawIfApplicable(*CanvasGUI.getRenderTarget());
            drawCanvasesToScreen();

            ImGui::SFML::Render(Window);

            Window.display();

            ClockHUD.endFrame();


            DeltaTime = DeltaTimeClock.restart();
        }
    }

    ImGui::SFML::Shutdown();
}

////////////////////////////////////////////////////////////

void fi::Engine::handleDefaultInput()
{
    if ((Input.check(fi::key::LAlt, fi::KEYSTATE_DOWN)) || (Input.check(fi::key::RAlt, fi::KEYSTATE_DOWN)))
    {
        if (Input.check(fi::key::Enter, fi::KEYSTATE_INITIAL_DOWN))
        {
            toggleFullscreen();
        }
    }

    if (Input.check("frame-clock-toggle"))
    {
        ClockHUD.DrawEnabled = !ClockHUD.DrawEnabled;
    }

    if (Input.check("frame-clock-reset"))
    {
        ClockHUD.last_n_second_clock.clear();
    }

    if (Input.check(fi::key::code::Escape))
    {
        if (getWindowStyle() == sf::Style::Fullscreen)
        {
            toggleFullscreen();
        }
    }

    if (Input.check("quit"))
    {
        QuitFlag = true;
    }
}

////////////////////////////////////////////////////////////

void fi::Engine::executeResize(float x, float y)
{
    Input.resetRecordedInput(true);

    auto settings = sf::ContextSettings(getWindow().getSettings());
    CanvasWorld.create((int)x, (int)y, settings);
    CanvasWorld.setSmooth(true);

    settings.antialiasingLevel = 0; // anti aliasing on texture when rendering text looks blurry
    this->CanvasGUI.create((int)x, (int)y, settings);
    CanvasWorld.setSmooth(false); // same deal, makes the text look more blurry if smooth==true

    sf::View WindowView;
    WindowView.reset(sf::FloatRect((float)0, (float)0, x, y));
    Window.setView(WindowView);

    for (int i = 0; i < Ticks.size(); i++)
    {
        Ticks[i].init(); // delay the tick when resized, seems right to me
    }

    Plugins.execute(EVENT_RESIZE);
}

////////////////////////////////////////////////////////////

void fi::Engine::initJSON()
{
    auto Path = OS.getPathToExecutableDirectory();
    jsonConfig = fi::jsonLoad(Path + "settings-default.json", true);
}

////////////////////////////////////////////////////////////

void fi::Engine::initWindow(unsigned int AALevel, std::string WindowTitle, sf::Uint32 WindowStyle, int xPosition, int yPosition, int Width, int Height, bool CursorGrabbed, int FrameLimit, bool SuspendOnLostFocus)
{
    jsonConfig["general"]["antialiasing-level"] = AALevel;
    jsonConfig["general"]["windowed-width"] = Width;
    jsonConfig["general"]["windowed-height"] = Height;
    jsonConfig["general"]["windowed-x-position"] = xPosition;
    jsonConfig["general"]["windowed-y-position"] = yPosition;
    jsonConfig["general"]["framerate-limit"] = FrameLimit;
    jsonConfig["general"]["title"] = WindowTitle;
    jsonConfig["general"]["title-info"] = WindowTitle;
    jsonConfig["general"]["mouse-grab"] = CursorGrabbed;
    jsonConfig["general"]["suspend-on-lost-focus"] = SuspendOnLostFocus;

    initWindow();
}

////////////////////////////////////////////////////////////

void fi::Engine::initWindow()
{
    auto AALevel = jsonConfig["general"]["antialiasing-level"].get<int>();
    auto xWindowSize = jsonConfig["general"]["windowed-width"].get<int>();
    auto yWindowSize = jsonConfig["general"]["windowed-height"].get<int>();
    auto xWindowPosition = jsonConfig["general"]["windowed-x-position"].get<int>();
    auto yWindowPosition = jsonConfig["general"]["windowed-y-position"].get<int>();
    auto FramerateLimit = jsonConfig["general"]["framerate-limit"].get<int>();
    auto WindowTitle = jsonConfig["general"]["title"].get<std::string>();
    auto WindowTitleInfo = jsonConfig["general"]["title-info"].get<bool>();
    auto CursorGrabbed = jsonConfig["general"]["mouse-grab"].get<bool>();
    auto SuspendOnLostFocus = jsonConfig["general"]["suspend-on-lost-focus"].get<bool>();
    auto WindowStyle = getWindowStyle();

    // ----

    sf::ContextSettings ContextSettings;
    ContextSettings.depthBits = 24; // leave as 24, 32 causes bug on my windows machine where the fps cap is 60 rather than 144. weird i know
    ContextSettings.antialiasingLevel = AALevel;
    ContextSettings.stencilBits = 0;

    if (Window.isOpen())
    {
        Window.close();
    }

    if (WindowStyle == sf::Style::Fullscreen)
    {
        xWindowPosition = 0;
        yWindowPosition = 0;
        xWindowSize = sf::VideoMode::getDesktopMode().width;
        yWindowSize = sf::VideoMode::getDesktopMode().height;
        Window.create(sf::VideoMode::getDesktopMode(), WindowTitle, WindowStyle, ContextSettings);
    }
    else
    {
        if (xWindowSize == -9999)
        {
            xWindowSize = sf::VideoMode::getDesktopMode().width;
        }
        if (yWindowSize == -9999)
        {
            yWindowSize = sf::VideoMode::getDesktopMode().height;
        }

        // ---- constrain width and height to desktop size, so as to not create window larger than screen
        xWindowSize = std::min(sf::VideoMode::getDesktopMode().width, (unsigned) xWindowSize);
        yWindowSize = std::min(sf::VideoMode::getDesktopMode().height, (unsigned) yWindowSize);

        Window.create(sf::VideoMode((unsigned)xWindowSize, (unsigned)yWindowSize), WindowTitle, WindowStyle, ContextSettings);


        // ---- set window position
        if (xWindowPosition == -9999) // hack window position was not explicitly set through json
        {
            xWindowPosition = Window.getPosition().x;
        }
        if (yWindowPosition == -9999) // hack window position was not explicitly set through json
        {
            yWindowPosition = Window.getPosition().y;
        }

        Window.setPosition(sf::Vector2i(xWindowPosition, yWindowPosition));
    }

    Window.setTitle(WindowTitle);

    if (CursorVisible)
    {
        Window.setMouseCursorVisible(true);

        for (int i = 0; i < 4; i++)
            Cursors[i].set(sf::StandardCursor::TYPE(i));
        if (ActiveCursorIndex != -1)
            setCursorStyle(sf::StandardCursor::TYPE(ActiveCursorIndex));
    }
    else
    {
        Window.setMouseCursorVisible(false);
    }

    Window.setMouseCursorGrabbed(CursorGrabbed);

    std::string IconPath = OS.getPathToExecutableDirectory() + "/images/icon.png";
    if (OS.fileExists(IconPath))
    {
        setWindowIcon(IconPath);
    }

    if (FramerateLimit <= 0) // shouldn't mix the two -- if not framerate limit than vsync on, and vice versa
    {
        Window.setVerticalSyncEnabled(true);
    }
    else
    {
        Window.setVerticalSyncEnabled(false);
        Window.setFramerateLimit((unsigned) FramerateLimit);
    }

    executeResize(xWindowSize, yWindowSize); // so plugins know what is up

    ImGui::SFML::Init(Window); // todo can i call init multiple times?

    if (!QuitFlag) // possible QuitFlag==true if we are creating this window to display an error (ie: invalid json config)
    {
        //Input.record(); // lazy fix to what i believe is an issue with sfml window, where it won't report the correct size until the first poll event
    }

    Window.clear(BackgroundColor);
    Window.display();


    Engine::instance().CurrentlySuspended = false;
}

////////////////////////////////////////////////////////////

void fi::Engine::initCanvases()
{
    auto Default = getConfig()["controls"]["default-camera-zoom-level"].get<int>();
    auto Lowest = getConfig()["controls"]["lowest-camera-zoom-level"].get<int>();
    auto Highest = getConfig()["controls"]["highest-camera-zoom-level"].get<int>();
    CanvasWorld.setZoomLevels(Default, Lowest, Highest);
    CanvasGUI.setZoomLevels(0, 0, 0);
}

////////////////////////////////////////////////////////////

void fi::Engine::initTicks()
{
    for (auto jsonTickData : getConfig()["ticks"])
    {
        std::string id = jsonTickData["id"].get<std::string>();
        TickIdToIndexMap[id] = (signed)Ticks.size();

        double TicksPerSecond = jsonTickData["ticks-per-second"].get<double>();
        fi::Tick t;
        t.init(id, TicksPerSecond, false, (int)Ticks.size());
        TickIndexToIdMap[(int)Ticks.size()] = id;
        Ticks.push_back(t);
    }
}

////////////////////////////////////////////////////////////

void fi::Engine::setCursorStyle(const sf::StandardCursor::TYPE Type)
{
    if (CursorVisible)
    {
        if (ActiveCursorIndex != (int)Type)
        {
            Cursors[Type].set(Window.getSystemHandle());
            ActiveCursorIndex = Type;
        }
    }
}

////////////////////////////////////////////////////////////

void fi::Engine::setCursorGrabbed(bool Value)
{
    this->CursorGrabbed = Value;
    Window.setMouseCursorGrabbed(Value);
}

////////////////////////////////////////////////////////////

void fi::Engine::setCursorVisible(bool Value)
{
    this->CursorVisible = Value;
    Window.setMouseCursorVisible(Value);
}

////////////////////////////////////////////////////////////

bool fi::Engine::setWindowIcon(std::string FullPathToIcon)
{
    sf::Image Icon;
    bool IconLoaded = Icon.loadFromFile(FullPathToIcon);
    if (IconLoaded)
    {
        Window.setIcon(Icon.getSize().x, Icon.getSize().y, Icon.getPixelsPtr());
    }

    return IconLoaded;
}

////////////////////////////////////////////////////////////

bool fi::Engine::isFullscreen()
{
    return (getWindowStyle() == sf::Style::Fullscreen);
}

////////////////////////////////////////////////////////////

void fi::Engine::toggleFullscreen()
{
    auto WindowStyle = getWindowStyle();
    if (WindowStyle == sf::Style::Fullscreen)
    {
        jsonConfig["general"]["window-style"] = "windowed";
    }
    else
    {
        jsonConfig["general"]["window-style"] = "fullscreen";
    }

    initWindow();

    Input.trigger(fi::key::Enter); // can miss the key up when reinitializing window
}

////////////////////////////////////////////////////////////

sf::Uint8 fi::Engine::getWindowStyle()
{
    sf::Uint8 WindowStyle = sf::Style::Default;
    std::string WindowStyleString = fi::stringDowncase(jsonConfig["general"]["window-style"].get<std::string>());
    if (WindowStyleString == "none")
    {
        WindowStyle = 0;
    }
    else if (WindowStyleString == "close")
    {
        WindowStyle = sf::Style::Close;
    }
    else if ((WindowStyleString == "borderless") || (WindowStyleString == "borderless-windowed") || (WindowStyleString == "fullscreen"))
    {
        WindowStyle = sf::Style::Fullscreen;
    }
    else if (WindowStyleString == "resize")
    {
        WindowStyle = sf::Style::Resize;
    }
    else if (WindowStyleString == "titlebar")
    {
        WindowStyle = sf::Style::Titlebar;
    }

    return WindowStyle;
}

////////////////////////////////////////////////////////////

bool fi::Engine::getCursorGrabbed()
{
    return CursorGrabbed;
}

////////////////////////////////////////////////////////////

bool fi::Engine::getCursorVisible()
{
    return CursorVisible;
}

////////////////////////////////////////////////////////////

void fi::Engine::setBackgroundColor(sf::Color Color)
{
    BackgroundColor = Color;
}

////////////////////////////////////////////////////////////

void fi::Engine::setBackgroundColor(sf::Uint8 r, sf::Uint8 g, sf::Uint8 b)
{
    BackgroundColor = sf::Color(r, g, b);
}

////////////////////////////////////////////////////////////

sf::Time fi::Engine::getDeltaTime()
{
    return DeltaTime;
}

////////////////////////////////////////////////////////////

void fi::Engine::saveJSONConfig()
{
    getInput().saveKeyRebindings();

    std::ofstream file(OS.getPathToExecutableDirectory() + "settings-custom.json");
    file << jsonConfig;
}

////////////////////////////////////////////////////////////

void fi::Engine::updateWindowTitleIfApplicable()
{
    auto WindowTitleInfo = jsonConfig["general"]["title-info"].get<bool>();

    if (WindowTitleInfo)
    {
        std::stringstream NewTitle;
        NewTitle << jsonConfig["general"]["title"].get<std::string>();
        NewTitle << " -- FPS:  ";
        NewTitle << (int)ClockHUD.m_clock.getFramesPerSecond();
        NewTitle << " -- x:  ";
        NewTitle << getMouseWorldPosition2f().x;
        NewTitle << ", y:  ";
        NewTitle << getMouseWorldPosition2f().y;
        Window.setTitle(NewTitle.str());
    }
}

////////////////////////////////////////////////////////////

void fi::Engine::drawCanvasesToScreen()
{
    CanvasWorld.display();
    CanvasGUI.display();

    sf::Sprite spr;
    spr.setTexture(*CanvasWorld.getTexture());
    Window.draw(spr);

    spr.setTexture(*CanvasGUI.getTexture());
    Window.draw(spr);
}

////////////////////////////////////////////////////////////

bool fi::Engine::isSuspended()
{
    return CurrentlySuspended;
}

////////////////////////////////////////////////////////////

fi::Tick *fi::Engine::getTick(int TickID)
{
    return &Ticks[TickID];
}

////////////////////////////////////////////////////////////

fi::Tick *fi::Engine::getTick(std::string TickID)
{
    int TickIndex = TickIdToIndexMap[TickID];
    return &Ticks[TickIndex];
}

////////////////////////////////////////////////////////////

void fi::Engine::stepTicks()
{
    auto FrameCount = getFrameCount();
    for (int i = 0; i < Ticks.size(); i++)
    {
        Ticks[i].stepIfApplicable(FrameCount);
    }
}

////////////////////////////////////////////////////////////

sf::Uint64 fi::Engine::getFrameCount()
{
    return ClockHUD.m_clock.getTotalFrameCount();
}

////////////////////////////////////////////////////////////

fi::Engine &fi::getEngine()
{
    return fi::Engine::instance();
}

////////////////////////////////////////////////////////////

nlohmann::json &fi::getConfig()
{
    return fi::Engine::instance().jsonConfig;
}

////////////////////////////////////////////////////////////

fi::Operating_System &fi::getOS()
{
    return fi::Engine::instance().OS;
}

////////////////////////////////////////////////////////////

fi::Random_Generator &fi::getRandom()
{
    return fi::Engine::instance().RNG;
}
////////////////////////////////////////////////////////////

fi::Audio fi::getAudio()
{
    return fi::Engine::instance().Audio;
}

////////////////////////////////////////////////////////////

sf::RenderWindow &fi::getWindow()
{
    return fi::Engine::instance().Window;
}

////////////////////////////////////////////////////////////

fi::Canvas &fi::getCanvasWorld()
{
    return fi::Engine::instance().CanvasWorld;
}

////////////////////////////////////////////////////////////

fi::Canvas &fi::getCanvasGUI()
{
    return fi::Engine::instance().CanvasGUI;
}

////////////////////////////////////////////////////////////

sf::Time fi::getDeltaTime()
{
    return fi::Engine::instance().getDeltaTime();
}

////////////////////////////////////////////////////////////

sf::Clock &fi::getClock()
{
    return fi::Engine::instance().Clock;
}

////////////////////////////////////////////////////////////

fi::Plugin_Manager &fi::getPlugins()
{
    return fi::Engine::instance().Plugins;
}

////////////////////////////////////////////////////////////

fi::Input &fi::getInput()
{
    return fi::Engine::instance().Input;
}

////////////////////////////////////////////////////////////

fi::Tick *fi::getTick(std::string TickID)
{
    return fi::Engine::instance().getTick(TickID);
}

////////////////////////////////////////////////////////////

fi::Tick *fi::getTick(int TickID)
{
    return fi::Engine::instance().getTick(TickID);
}

////////////////////////////////////////////////////////////

sf::Vector2i fi::getMouseWorldPosition2i()
{
    return getInput().MouseWorldPosition;
}

////////////////////////////////////////////////////////////

sf::Vector2i fi::getMouseWindowPosition2i()
{
    return getInput().MouseWindowPosition;
}

////////////////////////////////////////////////////////////

sf::Vector2f fi::getMouseWorldPosition2f()
{
    return convert(getInput().MouseWorldPosition);
}

////////////////////////////////////////////////////////////

sf::Vector2f fi::getMouseWindowPosition2f()
{
    return convert(getInput().MouseWindowPosition);
}

////////////////////////////////////////////////////////////

bool isSuspended()
{
    return fi::Engine::instance().isSuspended();
}

////////////////////////////////////////////////////////////

int getCurrentProgramState()
{
    return fi::Engine::instance().Plugins.getCurrentProgramStateIndex();
}

////////////////////////////////////////////////////////////

void fi::setCursorStyle(const sf::StandardCursor::TYPE Type)
{
    fi::getEngine().setCursorStyle(Type);
}

////////////////////////////////////////////////////////////
// common to all apps
fi::Tick *getCoreTick()
{
    return fi::Engine::instance().getTick("core");
}

