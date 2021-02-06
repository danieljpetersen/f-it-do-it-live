#include <SFML/Window/Event.hpp>
#include <iostream>
#include "input.h"
#include "engine.h"

////////////////////////////////////////////////////////////

void fi::Input::init()
{
	RawUserInput = "";
	KeyStates.clear();
	KeyBindings.clear();
	NumberKeysPressed.clear();
	KeyStates.resize(fi::key::NUMBER_OF_FI_KEY_CODES);
	DOUBLE_CLICK_TIME = fi::getConfig()["general"]["double-click-time"].get<int>();
    MouseIdleTime = sf::Time::Zero;
	
	bindKeysBasedOffJSON();
	resetRecordedInput(true);
}

////////////////////////////////////////////////////////////

bool fi::Input::record()
{
	resetRecordedInput(false);
	
	MouseWindowPositionOnLastLoop = MouseWindowPosition;
	MouseWorldPositionOnLastLoop = MouseWorldPosition;
	
	MouseWindowPosition = sf::Mouse::getPosition(getWindow());
	sf::Vector2f MousePos = fi::getCanvasWorld().mapPixelToCoords(MouseWindowPosition);
	MouseWorldPosition.x = (int)MousePos.x;
	MouseWorldPosition.y = (int)MousePos.y;

	if (MouseWindowPosition == MouseWindowPositionOnLastLoop)
    {
        MouseIdleTime += fi::getDeltaTime();
    }
	else
    {
	    MouseIdleTime = sf::Time::Zero;
    }
	
	sf::Event Event;
	while (Engine::instance().Window.pollEvent(Event))
	{
        ImGui::SFML::ProcessEvent(Event);

		if (Event.type == sf::Event::Closed)
		{
			return true;
		}
		if (Event.type == sf::Event::TextEntered)
		{
			RawUserInput.insert(RawUserInput.getSize(), Event.text.unicode);
		}
		else if (Event.type == sf::Event::LostFocus)
		{
			if (Engine::instance().SuspendOnLostFocus)
			{
				Engine::instance().CurrentlySuspended = true;
			}

			_CursorGrabbedBeforeLostFocus = fi::Engine::instance().getCursorGrabbed();

			Engine::instance().setCursorGrabbed(false);

			resetRecordedInput(true);
		}
		else if (Event.type == sf::Event::GainedFocus)
		{
			Engine::instance().CurrentlySuspended = false;
			Engine::instance().setCursorGrabbed(_CursorGrabbedBeforeLostFocus);
            Engine::instance().UnsuspendThisFrame = true;
		}
		else if (Event.type == sf::Event::MouseWheelMoved)
		{
			if (Event.mouseWheel.delta > 0)
			{
				KeyStates[fi::key::ScrollUp].States[KEYSTATE_INITIAL_DOWN] = true;
			}
			else if (Event.mouseWheel.delta < 0)
			{
				KeyStates[fi::key::ScrollDown].States[KEYSTATE_INITIAL_DOWN] = true;
			}
		}
		else if (Event.type == sf::Event::Resized)
		{
			fi::Engine::instance().executeResize();
		}
		else
		{
			if (Event.type == sf::Event::MouseButtonPressed)
			{
				int KeyIndex = sfMouseButtonToFiKeyMapping[Event.mouseButton.button];
				KeyStates[KeyIndex].States[KEYSTATE_INITIAL_DOWN] = true;
				KeyStates[KeyIndex].States[KEYSTATE_DOWN] = true;
				
				MouseWorldPositionOnLastMouseDown = MouseWorldPosition;
				MouseWindowPositionOnLastMouseDown = MouseWindowPosition;
				
				if (Engine::instance().Clock.getElapsedTime().asMilliseconds() - KeyStates[KeyIndex].TimeOfLastInitialDown.asMilliseconds() < DOUBLE_CLICK_TIME)
				{
					KeyStates[KeyIndex].States[KEYSTATE_DOUBLE_TAPPED] = true;
				}
				
				KeyStates[KeyIndex].TimeOfLastInitialDown = fi::Engine::instance().Clock.getElapsedTime();
			}
			else if (Event.type == sf::Event::MouseButtonReleased)
			{
				int KeyIndex = sfMouseButtonToFiKeyMapping[Event.mouseButton.button];
				KeyStates[KeyIndex].States[KEYSTATE_DOWN] = false;
				KeyStates[KeyIndex].States[KEYSTATE_RELEASED] = true;
			}
			else if (Event.type == sf::Event::KeyPressed)
			{
				int KeyIndex = sfKeyboardKeyToFiKeyMapping[Event.key.code];
				KeyStates[KeyIndex].States[KEYSTATE_INITIAL_DOWN] = true;
				KeyStates[KeyIndex].States[KEYSTATE_DOWN] = true;
				
				if (fi::Engine::instance().Clock.getElapsedTime().asMilliseconds() - KeyStates[KeyIndex].TimeOfLastInitialDown.asMilliseconds() < DOUBLE_CLICK_TIME)
				{
					KeyStates[KeyIndex].States[KEYSTATE_DOUBLE_TAPPED] = true;
				}
				else
				{
					KeyStates[KeyIndex].TimeOfLastInitialDown = fi::Engine::instance().Clock.getElapsedTime();
				}
			}
			else if (Event.type == sf::Event::KeyReleased)
			{
				int KeyIndex = sfKeyboardKeyToFiKeyMapping[Event.key.code];
				KeyStates[KeyIndex].States[KEYSTATE_DOWN] = false;
				KeyStates[KeyIndex].States[KEYSTATE_RELEASED] = true;
			}
		}
	}

	// todo? is this still a thing?
	//if (getWindow().getSize() != sf::Vector2u(getEngine().WindowSize.x, getEngine().WindowSize.y))
	//{
	//	// weird issue on windows where transitioning into fullscreen the reporting for the window size seems to be delayed
	//	// results in the window size not being correct.  we're just going to explicitly check to make sure every frame, fuck it
	//	fi::Engine::instance().executeResize(getWindow().getSize().x, getWindow().getSize().y);
	//}
	
	return false;
}

////////////////////////////////////////////////////////////

bool fi::Input::check(std::string BindingName)
{
	for (int i = 0; i < KeyBindings[BindingName].size(); i++)
	{
		int KeyIndex = KeyBindings[BindingName][i].fiKey;
		int StateIndex = KeyBindings[BindingName][i].OnKeyState;
		
		if (check(KeyIndex, StateIndex))
			return true;
	}
	
	return false;
}

////////////////////////////////////////////////////////////

bool fi::Input::check(int fiKey)
{
	return check(fiKey, KEYSTATE_INITIAL_DOWN);
}

////////////////////////////////////////////////////////////

bool fi::Input::check(int fiKey, int KeyState)
{
	return KeyStates[fiKey].States[KeyState];
}

////////////////////////////////////////////////////////////

bool fi::Input::checkEitherShift()
{
	return checkEitherShift(KEYSTATE_INITIAL_DOWN);
}

////////////////////////////////////////////////////////////

bool fi::Input::checkEitherShift(int Keystate)
{
	if (check(key::LShift, Keystate))
	{
		return true;
	}
	if (check(key::RShift, Keystate))
	{
		return true;
	}
	
	return false;
}

////////////////////////////////////////////////////////////

bool fi::Input::checkEitherCtrl()
{
	return checkEitherCtrl(KEYSTATE_INITIAL_DOWN);
}

////////////////////////////////////////////////////////////

bool fi::Input::checkEitherCtrl(int Keystate)
{
	if (check(key::LCtrl, Keystate))
	{
		return true;
	}
	if (check(key::RCtrl, Keystate))
	{
		return true;
	}
	
	return false;
}

////////////////////////////////////////////////////////////

void fi::Input::bindKey(std::string BindingName, int fiKey)
{
	KeyBindings[BindingName].push_back(Key_Binding(fiKey, KEYSTATE_INITIAL_DOWN));
}

////////////////////////////////////////////////////////////

void fi::Input::bindKey(std::string BindingName, int fiKey, int KeyState)
{
	KeyBindings[BindingName].push_back(Key_Binding(fiKey, KeyState));
}

////////////////////////////////////////////////////////////

void fi::Input::resetRecordedInput(bool ResetDownKeystate)
{
	RawUserInput.clear();
	NumberKeysPressed.clear();

	if (ResetDownKeystate)
	{
		for (int i = 0; i < KeyStates.size(); i++)
		{
			KeyStates[i].States[0] = false;
			KeyStates[i].States[1] = false;
			KeyStates[i].States[2] = false;
			KeyStates[i].States[3] = false;
			KeyStates[i].ArtificialTrigger = false;
			KeyStates[i].PreviousArtificialTrigger = false;
		}

		return;
	}
	
	for (int i = 0; i < KeyStates.size(); i++)
	{
		if (KeyStates[i].ArtificialTrigger)
		{
			KeyStates[i].States[KEYSTATE_RELEASED] = false;

			if (KeyStates[i].States[KEYSTATE_DOWN] != true)
			{
				KeyStates[i].States[KEYSTATE_DOWN] = true;
				KeyStates[i].States[KEYSTATE_INITIAL_DOWN] = true;
			}
			else
			{
				KeyStates[i].States[KEYSTATE_INITIAL_DOWN] = false;
			}
			
			if (Engine::instance().Clock.getElapsedTime().asMilliseconds() - KeyStates[i].TimeOfLastInitialDown.asMilliseconds() < DOUBLE_CLICK_TIME)
			{
				KeyStates[i].States[KEYSTATE_DOUBLE_TAPPED] = true;
			}
			else
			{
				KeyStates[i].States[KEYSTATE_DOUBLE_TAPPED] = false;
			}
			
			KeyStates[i].PreviousArtificialTrigger = true;
			KeyStates[i].ArtificialTrigger = false;
		}
		else if (KeyStates[i].PreviousArtificialTrigger)
		{
			KeyStates[i].PreviousArtificialTrigger = false;
			KeyStates[i].States[KEYSTATE_INITIAL_DOWN] = false;
			KeyStates[i].States[KEYSTATE_DOUBLE_TAPPED] = false;
			KeyStates[i].States[KEYSTATE_RELEASED] = true;
		}
		else
		{
			KeyStates[i].ArtificialTrigger = false;
			KeyStates[i].PreviousArtificialTrigger = false;
			KeyStates[i].States[KEYSTATE_INITIAL_DOWN] = false;
			KeyStates[i].States[KEYSTATE_RELEASED] = false;
			KeyStates[i].States[KEYSTATE_DOUBLE_TAPPED] = false;
		}
	}
}

////////////////////////////////////////////////////////////

void fi::Input::trigger(std::string BindingName)
{
	for (int i = 0; i < KeyBindings[BindingName].size(); i++)
	{
		trigger(KeyBindings[BindingName][i].fiKey);
	}
}

////////////////////////////////////////////////////////////

void fi::Input::trigger(int fiKey)
{
	KeyStates[fiKey].ArtificialTrigger = true;
}

////////////////////////////////////////////////////////////

sf::Time fi::Input::timeOfLastInitialDown(std::string BindingName)
{
	sf::Time LastTime = sf::Time::Zero;
	for (int i = 0; i < KeyBindings[BindingName].size(); i++)
	{
		int KeyIndex = KeyBindings[BindingName][i].fiKey;
		
		if (KeyStates[KeyIndex].TimeOfLastInitialDown > LastTime)
		{
			LastTime = KeyStates[KeyIndex].TimeOfLastInitialDown;
		}
	}
	
	return LastTime;
}

////////////////////////////////////////////////////////////

sf::Time fi::Input::timeOfLastInitialDown(int fiKey)
{
	return KeyStates[fiKey].TimeOfLastInitialDown;
}

////////////////////////////////////////////////////////////

void fi::Input::resetKey(int fiKey)
{
    KeyStates[fiKey].States[0] = false;
    KeyStates[fiKey].States[1] = false;
    KeyStates[fiKey].States[2] = false;
    KeyStates[fiKey].States[3] = false;
    KeyStates[fiKey].ArtificialTrigger = false;
    KeyStates[fiKey].PreviousArtificialTrigger = false;
    KeyStates[fiKey].TimeOfLastInitialDown = sf::Time::Zero;
}

////////////////////////////////////////////////////////////

void fi::Input::resetDoubleClickTimer(std::string BindingName)
{
	for (int i = 0; i < KeyBindings[BindingName].size(); i++)
	{
		int KeyIndex = KeyBindings[BindingName][i].fiKey;
		resetDoubleClickTimer(KeyIndex);
	}
}

////////////////////////////////////////////////////////////

void fi::Input::resetDoubleClickTimer(int fiKey)
{
	KeyStates[fiKey].TimeOfLastInitialDown = sf::Time::Zero;
}

////////////////////////////////////////////////////////////

void fi::Input::bindKeysBasedOffJSON()
{
	auto jsonStrToGrKey = generateMappings();
	
	auto jsonConfig = fi::Engine::instance().jsonConfig;
	auto jsonControls = jsonConfig["controls"].get<nlohmann::json>();
	auto jsonControlBindings = jsonControls["key-bindings"].get<nlohmann::json>();
	
	for (nlohmann::json::iterator aIterator = jsonControlBindings.begin(); aIterator != jsonControlBindings.end(); ++aIterator)
	{
		std::string CommandName = aIterator.key();
		auto KeyBindings = (*aIterator)["bindings"].get<nlohmann::json>();
		
		if (KeyBindings.size())
		{
			std::string KeyStateStr = (*aIterator)["state"].get<std::string>();
			int KeyState = KEYSTATE_INITIAL_DOWN;
			if (fi::stringDowncase(KeyStateStr) == "tap")
			{
				KeyState = KEYSTATE_INITIAL_DOWN;
			}
			else if (fi::stringDowncase(KeyStateStr) == "released")
			{
				KeyState = KEYSTATE_RELEASED;
			}
			else if (fi::stringDowncase(KeyStateStr) == "down")
			{
				KeyState = KEYSTATE_DOWN;
			}
			else if (fi::stringDowncase(KeyStateStr) == "double-tapped")
			{
				KeyState = KEYSTATE_DOUBLE_TAPPED;
			}
			
			for (nlohmann::json::iterator bIterator = KeyBindings.begin(); bIterator != KeyBindings.end(); ++bIterator)
			{
				std::string KeyString = (*bIterator).get<std::string>();
				int fiKey = jsonStrToGrKey[KeyString];

				bindKey(CommandName, fiKey, KeyState);
			}
		}
	}
}

////////////////////////////////////////////////////////////

void fi::Input::saveKeyRebindings()
{
	std::string keyBindings = "{ \"key-bindings\": {";

	for (auto Commands : KeyBindings)
	{
		std::string CommandName = Commands.first;

		keyBindings += "\"" + CommandName + "\": { \"state\": ";

		if (Commands.second.empty())
		{
			keyBindings += "\"tap\", \"bindings\": [] }, "; // fuck it default to tap with an empty array
		}

		bool AddedKeyState = false;
		for (auto KeyBinding : Commands.second)
		{
			if (AddedKeyState != true)
			{
				AddedKeyState = true;
				if (KeyBinding.OnKeyState == KEYSTATE_INITIAL_DOWN)
				{
					keyBindings += "\"tap\", \"bindings\": [ ";
				}
				else if (KeyBinding.OnKeyState == KEYSTATE_DOWN)
				{
					keyBindings += "\"down\", \"bindings\": [ ";
				}
				else if (KeyBinding.OnKeyState == KEYSTATE_DOUBLE_TAPPED)
				{
					keyBindings += "\"double-tapped\", \"bindings\": [ ";
				}
				else if (KeyBinding.OnKeyState == KEYSTATE_RELEASED)
				{
					keyBindings += "\"released\", \"bindings\": [ ";
				}
			}
			else
			{
				keyBindings += ",";
			}
			std::string jsonKeyStr = getJSONStrFromFiKey(KeyBinding.fiKey);

			keyBindings += "\"" + jsonKeyStr + "\"";
		}

		if (Commands.second.empty() != true)
		{
			keyBindings += " ] }, ";
		}
	}
	keyBindings.pop_back();// space
	keyBindings.pop_back();// comma
	keyBindings += " } }";

	nlohmann::json parsedJSON = nlohmann::json::parse(keyBindings);
	fi::getConfig()["controls"].update(parsedJSON);
}

////////////////////////////////////////////////////////////

std::string fi::Input::getJSONStrFromFiKey(int fiKey)
{
	for (auto jsonKey : jsonStrToFiKeyMapping)
	{
		if (jsonKey.second == fiKey)
		{
			return jsonKey.first;
		}
	}

	return "";
}

////////////////////////////////////////////////////////////

std::unordered_map<std::string, int> fi::Input::generateMappings()
{
	sfMouseButtonToFiKeyMapping.clear();
	sfKeyboardKeyToFiKeyMapping.clear();
	jsonStrToFiKeyMapping.clear();
	
	// ---- mouse
	jsonStrToFiKeyMapping["left-arrow"] = fi::key::LeftArrow;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Left] = fi::key::LeftArrow;
	
	jsonStrToFiKeyMapping["right-arrow"] = fi::key::RightArrow;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Right] = fi::key::RightArrow;
	
	jsonStrToFiKeyMapping["up-arrow"] = fi::key::UpArrow;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Up] = fi::key::UpArrow;
	
	jsonStrToFiKeyMapping["down-arrow"] = fi::key::DownArrow;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Down] = fi::key::DownArrow;

	jsonStrToFiKeyMapping["page-up"] = fi::key::PageUp;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::PageUp] = fi::key::PageUp;
	
	jsonStrToFiKeyMapping["page-down"] = fi::key::PageDown;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::PageDown] = fi::key::PageDown;

	jsonStrToFiKeyMapping["l-ctrl"] = fi::key::LCtrl;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::LControl] = fi::key::LCtrl;

	jsonStrToFiKeyMapping["r-ctrl"] = fi::key::RCtrl;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::RControl] = fi::key::RCtrl;

	jsonStrToFiKeyMapping["l-alt"] = fi::key::LAlt;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::LAlt] = fi::key::LAlt;

	jsonStrToFiKeyMapping["r-alt"] = fi::key::RAlt;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::RAlt] = fi::key::RAlt;

	jsonStrToFiKeyMapping["l-shift"] = fi::key::LShift;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::LShift] = fi::key::LShift;
	
	jsonStrToFiKeyMapping["r-shift"] = fi::key::RShift;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::RShift] = fi::key::RShift;
	
	jsonStrToFiKeyMapping["escape"] = fi::key::Escape;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Escape] = fi::key::Escape;
	
	jsonStrToFiKeyMapping["tilde"] = fi::key::Tilde;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Tilde] = fi::key::Tilde;

	jsonStrToFiKeyMapping["space"] = fi::key::Space;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Space] = fi::key::Space;
	
	jsonStrToFiKeyMapping["backspace"] = fi::key::Backspace;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Backspace] = fi::key::Backspace;
	
	jsonStrToFiKeyMapping["enter"] = fi::key::Enter;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Enter] = fi::key::Enter;
	
	jsonStrToFiKeyMapping["minus"] = fi::key::Minus;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Subtract] = fi::key::Minus;
	
	jsonStrToFiKeyMapping["plus"] = fi::key::Plus;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Add] = fi::key::Plus;
	
	jsonStrToFiKeyMapping["["] = fi::key::LBracket;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::LBracket] = fi::key::LBracket;
	
	jsonStrToFiKeyMapping["]"] = fi::key::RBracket;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::RBracket] = fi::key::RBracket;
	
	jsonStrToFiKeyMapping["/"] = fi::key::Forwardslash;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Slash] = fi::key::Forwardslash;
	
	jsonStrToFiKeyMapping["\\"] = fi::key::Backslash;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Backslash] = fi::key::Backslash;
	
	jsonStrToFiKeyMapping["num-pad-0"] = fi::key::Numpad0;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Numpad0] = fi::key::Numpad0;
	
	jsonStrToFiKeyMapping["num-pad-1"] = fi::key::Numpad1;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Numpad1] = fi::key::Numpad1;
	
	jsonStrToFiKeyMapping["num-pad-2"] = fi::key::Numpad2;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Numpad2] = fi::key::Numpad2;
	
	jsonStrToFiKeyMapping["num-pad-3"] = fi::key::Numpad3;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Numpad3] = fi::key::Numpad3;
	
	jsonStrToFiKeyMapping["num-pad-4"] = fi::key::Numpad4;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Numpad4] = fi::key::Numpad4;
	
	jsonStrToFiKeyMapping["num-pad-5"] = fi::key::Numpad5;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Numpad5] = fi::key::Numpad5;
	
	jsonStrToFiKeyMapping["num-pad-6"] = fi::key::Numpad6;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Numpad6] = fi::key::Numpad6;
	
	jsonStrToFiKeyMapping["num-pad-7"] = fi::key::Numpad7;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Numpad7] = fi::key::Numpad7;
	
	jsonStrToFiKeyMapping["num-pad-8"] = fi::key::Numpad8;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Numpad8] = fi::key::Numpad8;
	
	jsonStrToFiKeyMapping["num-pad-9"] = fi::key::Numpad9;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Numpad9] = fi::key::Numpad9;
	
	jsonStrToFiKeyMapping["num-0"] = fi::key::Num0;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Numpad0] = fi::key::Num0;
	
	jsonStrToFiKeyMapping["num-1"] = fi::key::Num1;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Num1] = fi::key::Num1;
	
	jsonStrToFiKeyMapping["num-2"] = fi::key::Num2;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Num2] = fi::key::Num2;
	
	jsonStrToFiKeyMapping["num-3"] = fi::key::Num3;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Num3] = fi::key::Num3;
	
	jsonStrToFiKeyMapping["num-4"] = fi::key::Num4;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Num4] = fi::key::Num4;
	
	jsonStrToFiKeyMapping["num-5"] = fi::key::Num5;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Num5] = fi::key::Num5;
	
	jsonStrToFiKeyMapping["num-6"] = fi::key::Num6;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Num6] = fi::key::Num6;
	
	jsonStrToFiKeyMapping["num-7"] = fi::key::Num7;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Num7] = fi::key::Num7;
	
	jsonStrToFiKeyMapping["num-8"] = fi::key::Num8;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Num8] = fi::key::Num8;
	
	jsonStrToFiKeyMapping["num-9"] = fi::key::Num9;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Num9] = fi::key::Num9;
	
	jsonStrToFiKeyMapping["a"] = fi::key::A;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::A] = fi::key::A;
	
	jsonStrToFiKeyMapping["b"] = fi::key::B;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::B] = fi::key::B;
	
	jsonStrToFiKeyMapping["c"] = fi::key::C;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::C] = fi::key::C;
	
	jsonStrToFiKeyMapping["d"] = fi::key::D;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::D] = fi::key::D;
	
	jsonStrToFiKeyMapping["e"] = fi::key::E;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::E] = fi::key::E;
	
	jsonStrToFiKeyMapping["f"] = fi::key::F;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::F] = fi::key::F;
	
	jsonStrToFiKeyMapping["g"] = fi::key::G;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::G] = fi::key::G;
	
	jsonStrToFiKeyMapping["h"] = fi::key::H;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::H] = fi::key::H;
	
	jsonStrToFiKeyMapping["i"] = fi::key::I;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::I] = fi::key::I;
	
	jsonStrToFiKeyMapping["j"] = fi::key::J;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::J] = fi::key::J;
	
	jsonStrToFiKeyMapping["k"] = fi::key::K;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::K] = fi::key::K;
	
	jsonStrToFiKeyMapping["l"] = fi::key::L;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::L] = fi::key::L;
	
	jsonStrToFiKeyMapping["m"] = fi::key::M;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::M] = fi::key::M;
	
	jsonStrToFiKeyMapping["n"] = fi::key::N;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::N] = fi::key::N;
	
	jsonStrToFiKeyMapping["o"] = fi::key::O;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::O] = fi::key::O;
	
	jsonStrToFiKeyMapping["p"] = fi::key::P;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::P] = fi::key::P;
	
	jsonStrToFiKeyMapping["q"] = fi::key::Q;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Q] = fi::key::Q;
	
	jsonStrToFiKeyMapping["r"] = fi::key::R;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::R] = fi::key::R;
	
	jsonStrToFiKeyMapping["s"] = fi::key::S;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::S] = fi::key::S;
	
	jsonStrToFiKeyMapping["t"] = fi::key::T;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::T] = fi::key::T;
	
	jsonStrToFiKeyMapping["u"] = fi::key::U;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::U] = fi::key::U;
	
	jsonStrToFiKeyMapping["v"] = fi::key::V;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::V] = fi::key::V;
	
	jsonStrToFiKeyMapping["w"] = fi::key::W;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::W] = fi::key::W;
	
	jsonStrToFiKeyMapping["x"] = fi::key::X;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::X] = fi::key::X;
	
	jsonStrToFiKeyMapping["y"] = fi::key::Y;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Y] = fi::key::Y;
	
	jsonStrToFiKeyMapping["z"] = fi::key::Z;
	sfKeyboardKeyToFiKeyMapping[sf::Keyboard::Key::Z] = fi::key::Z;
	
	jsonStrToFiKeyMapping["mouse-1"] = fi::key::Mouse1;
	sfMouseButtonToFiKeyMapping[sf::Mouse::Button::Left] = fi::key::Mouse1;
	
	jsonStrToFiKeyMapping["mouse-2"] = fi::key::Mouse2;
	sfMouseButtonToFiKeyMapping[sf::Mouse::Button::Right] = fi::key::Mouse2;
	
	jsonStrToFiKeyMapping["mouse-3"] = fi::key::Mouse3;
	sfMouseButtonToFiKeyMapping[sf::Mouse::Button::Middle] = fi::key::Mouse3;
	
	jsonStrToFiKeyMapping["mouse-4"] = fi::key::Mouse4;
	sfMouseButtonToFiKeyMapping[sf::Mouse::Button::XButton1] = fi::key::Mouse4;
	
	jsonStrToFiKeyMapping["mouse-5"] = fi::key::Mouse5;
	sfMouseButtonToFiKeyMapping[sf::Mouse::Button::XButton2] = fi::key::Mouse5;
	
	jsonStrToFiKeyMapping["scroll-up"] = fi::key::ScrollUp; // only 1 mapping, we just explicitly handle it in record...
	jsonStrToFiKeyMapping["scroll-down"] = fi::key::ScrollDown; // only 1 mapping, we just explicitly handle it in record...

	return jsonStrToFiKeyMapping;
}

////////////////////////////////////////////////////////////

std::string fi::Input::getRawInput()
{
    return RawUserInput;
}

////////////////////////////////////////////////////////////

sf::Time fi::Input::getMouseIdleTime()
{
    return MouseIdleTime;
}

////////////////////////////////////////////////////////////
