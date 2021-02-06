#ifndef FI_INPUT_H
#define FI_INPUT_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <unordered_map>
#include <vector>

////////////////////////////////////////////////////////////

// todo -- add more keys (ie:  f1, f2, etc) + sf::joystick

////////////////////////////////////////////////////////////

namespace fi
{
	const int KEYSTATE_INITIAL_DOWN = 0;
	const int KEYSTATE_DOWN = 1;
	const int KEYSTATE_RELEASED = 2;
	const int KEYSTATE_DOUBLE_TAPPED = 3;

	////////////////////////////////////////////////////////////
	
	// I'm doing this because I want to support Input.check(fi::Key::*) being any valid button and
	// sf::Mouse::Button, sf::Mouse::Wheel, sf::Keyboard::Key, and sf::Joystick are all separate enums...
	// Maybe there's a cleaner way to do it with templates, but this is fine
	namespace key
	{
		enum code // serves as an index into Input.KeyStates
		{
			Mouse1, Mouse2, Mouse3, Mouse4, Mouse5,
			ScrollUp, ScrollDown, Escape,
			LeftArrow, RightArrow, UpArrow, DownArrow,
			LCtrl, RCtrl, LShift, RShift, LAlt, RAlt, PageUp, PageDown,
			Tilde, Space, Backspace, Enter, Minus, Plus, LBracket, RBracket, Forwardslash, Backslash,
			Numpad0, Numpad1, Numpad2, Numpad3, Numpad4, Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
			Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
			A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
			NUMBER_OF_FI_KEY_CODES
		};
	}

	////////////////////////////////////////////////////////////
	
	class Key_State
	{
	public:
		Key_State()
		{
			States[KEYSTATE_INITIAL_DOWN] = false;
			States[KEYSTATE_DOWN] = false;
			States[KEYSTATE_RELEASED] = false;
			States[KEYSTATE_DOUBLE_TAPPED] = false;
			TimeOfLastInitialDown = sf::Time::Zero;
			ArtificialTrigger = false;
			PreviousArtificialTrigger = false; // for artificial trigger of KEYSTATE_RELEASED
		};
		
		bool States[4]; // array rather than an int because multiple can be true at once, ie: INITIAL_DOWN + DOWN
		bool ArtificialTrigger, PreviousArtificialTrigger;
		sf::Time TimeOfLastInitialDown;
	};
	
	////////////////////////////////////////////////////////////
	
	class Key_Binding
	{
	public:
		Key_Binding(int fiKey, int OnKeyState)
		{
			this->fiKey = fiKey;
			this->OnKeyState = OnKeyState;
		};
		
		int OnKeyState; // ie:  KEYSTATE_INITIAL_DOWN
		int fiKey; // ie:  Input.KeyStates[fiKey]
	};
	
	////////////////////////////////////////////////////////////

	class Input
	{
	private:
		sf::String RawUserInput; // All keys pressed since the last input check get appended to this string
		std::vector<Key_State> KeyStates; // todo -- dates back to like 2013. it works fine but the ideal implementation would have a map of the keys currently active rather than an array for KeyStates
        std::unordered_map<std::string, std::vector<Key_Binding>> KeyBindings;

        void resetRecordedInput(bool ResetDownKeystate);
		void bindKeysBasedOffJSON();
		void saveKeyRebindings();

		std::unordered_map<int, int> sfMouseButtonToFiKeyMapping;
		std::unordered_map<int, int> sfKeyboardKeyToFiKeyMapping;
		std::unordered_map<std::string, int> jsonStrToFiKeyMapping;
		std::string getJSONStrFromFiKey(int fiKey);

		// populates sfMouse/KeyboardToFiKeyMapping, as well as jsonStrToFiMapping (which is the return value)
		std::unordered_map<std::string, int> generateMappings();

		bool _CursorGrabbedBeforeLostFocus = false;
		int DOUBLE_CLICK_TIME; // set from json

	public:
		void init();
		bool record();
		
		void bindKey(std::string BindingName, int fiKey); // defaults to KEYSTATE_INITIAL_DOWN
		void bindKey(std::string BindingName, int fiKey, int KeyState);
		
		bool check(std::string BindingName);
		bool check(int fiKey); // defaults to KEYSTATE_INITIAL_DOWN
		bool check(int fiKey, int KeyState);
		
		bool checkEitherShift();
		bool checkEitherShift(int Keystate);
		bool checkEitherCtrl();
		bool checkEitherCtrl(int Keystate);

		// triggering follows same logic as regular keypresses
		// so an artificial trigger on two frames in a row:
		// (frame 1) initial_down + down -> (frame 2) down + double_tapped -> (frame 3) released
		void trigger(std::string BindingName);
		void trigger(int fiKey);
		
		void resetDoubleClickTimer(std::string BindingName);
		void resetDoubleClickTimer(int fiKey);
		sf::Time timeOfLastInitialDown(std::string BindingName);
		sf::Time timeOfLastInitialDown(int fiKey);

		void resetKey(int fiKey);
		
		sf::Vector2i MouseWindowPosition, MouseWorldPosition;
		sf::Vector2i MouseWindowPositionOnLastMouseDown, MouseWorldPositionOnLastMouseDown;
		sf::Vector2i MouseWindowPositionOnLastLoop, MouseWorldPositionOnLastLoop; // needed for camera mouse drag
        sf::Time MouseIdleTime;

		std::vector<int> NumberKeysPressed;

		std::string getRawInput();
        sf::Time getMouseIdleTime();

		friend class Engine;
	};

	////////////////////////////////////////////////////////////

    sf::Vector2i getMouseWorldPosition2i();
    sf::Vector2i getMouseWindowPosition2i();
    sf::Vector2f getMouseWorldPosition2f();
    sf::Vector2f getMouseWindowPosition2f();
}

#endif