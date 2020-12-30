#include "pauseable_clock.h"

namespace fi
{
	bool Pauseable_Clock::universalpause = false;

	void Pauseable_Clock::think()
	{
		if (universalpause)
		{
			pause();
		}
		else
		{
			start();
		}
	}

    ////////////////////////////////////////////////////////////

	void Pauseable_Clock::pause()
	{
		if (!paused)
		{
			paused = true;
			pcTime += sf::seconds(internalClock.getElapsedTime().asSeconds());
		}
	}

    ////////////////////////////////////////////////////////////

	void Pauseable_Clock::start()
	{
		if (paused)
		{
			paused = false;
			internalClock.restart();
		}
	}

    ////////////////////////////////////////////////////////////

	const sf::Time Pauseable_Clock::getElapsedTime()
	{
		if (!paused)
		{
			return sf::seconds(pcTime.asSeconds() + internalClock.getElapsedTime().asSeconds());
		}
		else
		{
			return pcTime;
		}
	}

    ////////////////////////////////////////////////////////////

	sf::Time Pauseable_Clock::restart(const bool pause)
	{
		sf::Time et = sf::seconds(pcTime.asSeconds() + internalClock.getElapsedTime().asSeconds());

		paused = pause;
		pcTime = sf::seconds(0);
		internalClock.restart();

		return et;
	}

    ////////////////////////////////////////////////////////////

	void Pauseable_Clock::universalPause()
	{
		if (!universalpause)
		{
			universalpause = true;
		}
	}

    ////////////////////////////////////////////////////////////

	void Pauseable_Clock::universalStart()
	{
		if (universalpause)
		{
			universalpause = false;
		}
	}

    ////////////////////////////////////////////////////////////

	Pauseable_Clock::Pauseable_Clock() :
			paused(false),
			pcTime(sf::seconds(0))
	{ }
}