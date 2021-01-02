#ifndef STANDARCURSOR_HPP
#define STANDARCURSOR_HPP

#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#ifdef SFML_SYSTEM_WINDOWS
#include <windows.h>
#elif defined(SFML_SYSTEM_LINUX)

#include <X11/cursorfont.h>
#include <X11/Xlib.h>

#else
#error This OS is not yet supported by the cursor library.
#endif

namespace sf
{
	class StandardCursor
	{
	private:
		bool Initialized;

		void free();

#ifdef SFML_SYSTEM_WINDOWS

		HCURSOR Cursor; /*Type of the Cursor with Windows*/

#else

		XID Cursor;
		Display* display = nullptr;

#endif
	public:
		enum TYPE{ WAIT, TEXT, NORMAL, HAND /*,...*/ };
		StandardCursor()
		{
#if defined(SFML_SYSTEM_LINUX)
			display = nullptr;
#endif
		}

		StandardCursor(const TYPE t);
		void set(const sf::WindowHandle& aWindowHandle) const;
		void set(const TYPE t);
		~StandardCursor();
	};
}

namespace fi
{
    void setCursorStyle(const sf::StandardCursor::TYPE Type);
}

#endif // STANDARCURSOR_HPP