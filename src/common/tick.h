#ifndef FI_TICK_H
#define FI_TICK_H

#include <chrono>
#include <set>
#include <climits>
#include "atomic_wrapper.h"
#include <SFML/Graphics.hpp>
#include "pauseable_clock.h"
#include "util.h"

bool isSuspended();
int getCurrentProgramState();

namespace fi
{
	class Tick
	{
	private:
		std::string Name;
		int TickIndex = -1;
        int TickCount = 0;

		Pauseable_Clock LocalClock;
		atomicwrapper<bool> StepThisFrame;
		std::set<int> ActiveProgramStates;
		int UniqueTokenPerFrame = -INT_MAX; // for ensuring step occurs once per frame, as we'll potentially be calling stepIfApplicable() multiple times per frame

		double TicksPerSecond = 0;
		double Interpolation = 0;
		double SkipTicks = 0;

		////////////////////////////////////////////////////////////

		double updateInterpolation()
		{
			std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
			auto duration = now.time_since_epoch();
			auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
			
			Interpolation = double((double)millis + (double)SkipTicks - (double)NextTickTime) / (double) SkipTicks;

			if (Interpolation > 1.3f)
			{
				Interpolation = 1.3f; // why isn't this capped at 1? wtf?
			}
			
			return Interpolation;
		}

		////////////////////////////////////////////////////////////

		bool stepIfApplicable(int FrameToken)
		{
			if (UniqueTokenPerFrame != FrameToken)
			{
				if (isActive())
				{
					UniqueTokenPerFrame = FrameToken;

					StepThisFrame._a.store(false);

                    LocalClock.think();

                    if (!LocalClock.isPaused())
                    {
                        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
                        auto duration = now.time_since_epoch();
                        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

                        if (millis >= NextTickTime)
                        {
                            TickCount++;

                            TimeOfLastTick = NextTickTime;

                            // it's single player, let's just reset it if it falls behind
                            if (NextTickTime + SkipTicks * 2 <= millis)
                            {
                                NextTickTime = millis;
                            }

                            NextTickTime += SkipTicks;

                            StepThisFrame._a.store(true);
                        }

                        updateInterpolation();
                    }
				}
			}

			return StepThisFrame._a.load();
		}

		////////////////////////////////////////////////////////////

	public:

		double NextTickTime, TimeOfLastTick = 0;

		void init()
		{
			init(Name, TicksPerSecond, isPaused(), TickIndex);
		}

		////////////////////////////////////////////////////////////

		void init(std::string TickName, double TicksPerSecond, bool IsPaused, int TickIndex)
		{
			this->Name = TickName;
			std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
			auto duration = now.time_since_epoch();
			auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
			this->SkipTicks = 1000.f / TicksPerSecond;
			this->TicksPerSecond = TicksPerSecond;
			this->TimeOfLastTick = millis;
			this->NextTickTime = millis + SkipTicks;
			this->TickIndex = TickIndex;
			this->LocalClock.restart();
			this->Interpolation = 0;

			StepThisFrame._a.store(false);

			if (IsPaused)
			{
				this->LocalClock.pause();
			}
			else
			{
				this->LocalClock.start();
			}
		}

		////////////////////////////////////////////////////////////

		double getDurationMS(int NumTicks)
		{
			return ((double)NumTicks/getTicksPerSecond()) * 1000;
		}

		////////////////////////////////////////////////////////////

		void swapTimestep(double NewTicksPerSecond)
		{
			double NewSkipTicks;
			if (NewTicksPerSecond == 0)
			{
				NewSkipTicks = 1; // probably fine?  prevent divide by 0 here as well for interpolation
			}
			else
			{
				NewSkipTicks = 1000. / NewTicksPerSecond;
			}

			NextTickTime = NextTickTime - SkipTicks + NewSkipTicks;
			SkipTicks = NewSkipTicks;
		}

        ////////////////////////////////////////////////////////////

        void setTickCount(int Value)
        {
		    this->TickCount = Value;
        }

		////////////////////////////////////////////////////////////

		std::string getName()
		{
			return Name;
		}
		
		////////////////////////////////////////////////////////////

		int getTickIndex()
		{
			return TickIndex;
		}
		
		////////////////////////////////////////////////////////////

		int getTickCount()
		{
			return TickCount;
		}

		////////////////////////////////////////////////////////////
		
		double getInterpolation()
		{
			return Interpolation;
		}
		
		////////////////////////////////////////////////////////////
		
		bool isPaused()
		{
			return this->LocalClock.isPaused();
		}

		////////////////////////////////////////////////////////////

		bool isActive()
		{
			if (this->ActiveProgramStates.empty())
				return false;

			int CurrentProgramState = getCurrentProgramState();

			return (this->isActiveDuringProgramState(CurrentProgramState));
		}

		////////////////////////////////////////////////////////////

		bool isStepThisFrame()
		{
			return StepThisFrame._a.load();
		}
		////////////////////////////////////////////////////////////
		
		sf::Vector2f interpolate(sf::Vector2f CurrentPosition, sf::Vector2f PreviousPosition)
		{
			sf::Vector2f DrawPosition;
			DrawPosition.x = static_cast<float>(PreviousPosition.x + (CurrentPosition.x - PreviousPosition.x) * Interpolation);
			DrawPosition.y = static_cast<float>(PreviousPosition.y + (CurrentPosition.y - PreviousPosition.y) * Interpolation);

			return DrawPosition;
		}

		////////////////////////////////////////////////////////////

		double getTicksPerSecond()
		{
			return TicksPerSecond;
		}

		////////////////////////////////////////////////////////////

		void pauseTimestep()
		{
			this->LocalClock.pause();
		}

		////////////////////////////////////////////////////////////

		void unpauseTimestep()
		{
			this->LocalClock.start();
		}

		////////////////////////////////////////////////////////////

        bool isActiveDuringProgramState(int ProgramStateIndex)
        {
            return ActiveProgramStates.find(ProgramStateIndex) != ActiveProgramStates.end();
        }

        ////////////////////////////////////////////////////////////

        friend class Engine;
        friend class Program_State_Builder;
    };
}

fi::Tick *getCoreTick(); // common to all apps

#endif