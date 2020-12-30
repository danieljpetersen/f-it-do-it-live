#ifndef FI_PLUGINS_H
#define FI_PLUGINS_H

#include <SFML/Graphics.hpp>
#include "atomic_wrapper.h"
#include "cereal/archives/binary.hpp"
#include "non_movable.h"
#include "non_copyable.h"
#include "util.h"

namespace fi
{
	////////////////////////////////////////////////////////////
	// a shell of empty functions which are overridable and automatically called at their implied points
	class Plugin_Base
	{
	public:
		int PluginIndex = -1;
		bool Enabled = false;
		bool ShouldUpdate = false, ShouldUpdateDefaultValue = false; // is this plugin called every update, or only updates at which ShouldUpdate == true?

		virtual void onProgramStart() {};
		virtual void onResize() {};
		virtual void onUpdate() {};
		virtual void onDraw() {};

		virtual void onEnableChange(bool Enabling) {};

		virtual void afterLoad() {};
		virtual void save( cereal::BinaryOutputArchive & ar ) {}; // see note below
		virtual void load( cereal::BinaryInputArchive & ar ) {}; // see note below
		// NOTE: you could override save/load manually, but it's easier to use FI_MODULE_SERIALIZATION_BOILERPLATE
		// then define a regular cereal function inside your plugin like <template Archive>serialize(Archive & Ar).  IE:
			//  FI_MODULE_SERIALIZATION_BOILERPLATE
			//
			// 	template <class Archive>
			// 	void serialize( Archive & ar )
			// 	{
			// 		ar(myData, myData2);
			// 	}
		// this way you only need to define your variables to serialize once
	};

	////////////////////////////////////////////////////////////
    // ProgramState is a set of plugins executing at once
	class Program_State_Builder
	{
	public:
		Program_State_Builder(int ProgramStateIndex);

		Program_State_Builder withPlugin(fi::Plugin_Base *Plugin, bool DefaultUpdateValue);
        Program_State_Builder withTick(std::string TickID);

        // do note that the other ProgramState needs to be already defined.  @refactor fix via delay of processing
		Program_State_Builder withProgramState(int ProgramStateIndex);

	private:
		int mProgramStateIndex = -1;
	};

	////////////////////////////////////////////////////////////
    // todo this thing could use a refactor + consider merging with a thread pool job system
	class Plugin_Manager : public Non_Copyable, public Non_Movable
	{
	public:
		std::vector<fi::Plugin_Base*> Plugins;
		bool ForceExecuteEnables = false;

        fi::Program_State_Builder defineProgramState(int &ProgramStateIndex)
		{
			ProgramStateIndex = (signed)ProgramStates.size();
			ProgramStates.resize(ProgramStates.size() + 1);

			ProgramStateIndexesActivePerEachState.resize(ProgramStateIndexesActivePerEachState.size() + 1);
			ProgramStateIndexesActivePerEachState[ProgramStateIndex].push_back(ProgramStateIndex);
			return Program_State_Builder(ProgramStateIndex);
		}

		void push(int ProgramStateIndex)
		{
			assert(ProgramStateIndex != -1);
			push(ProgramStates[ProgramStateIndex]);
		}

		void pop()
		{
			if (!StateStack.empty())
			{
				StateStack.pop_back();
			}
		}

        void clearStackThenTransitionToProgramState_ForceExecuteEnables(int ProgramStateIndex)
        {
		    ForceExecuteEnables = true;
            StateStack.clear();
            push(ProgramStateIndex);
        }

		void clearStackThenTransitionToProgramState(int ProgramStateIndex)
		{
			StateStack.clear();
			push(ProgramStateIndex);
		}

		bool isProgramStateActive(int ProgramStateIndex)
		{
			int _CurrentProgramStateIndex = CurrentProgramStateIndex._a.load();
			if (_CurrentProgramStateIndex == ProgramStateIndex)
			{
				return true;
			}

			// ie:  withProgramState(...) was used.
			for (int i = 0; i < ProgramStateIndexesActivePerEachState[_CurrentProgramStateIndex].size(); i++)
			{
				if (ProgramStateIndexesActivePerEachState[_CurrentProgramStateIndex][i] == ProgramStateIndex)
				{
					return true;
				}
			}

			return false;
		}

        int getCurrentProgramState()
        {
            return CurrentProgramStateIndex._a.load();
        }

		void flagUpdate()
		{
			ShouldUpdate = true;
		}

		atomicwrapper<int> CurrentProgramStateIndex;

	private:
		std::vector<std::vector<int>> ProgramStates; // all possible states
		std::vector<std::vector<int>> StateStack;
		std::vector<std::vector<int>> ProgramStateIndexesActivePerEachState; // ie:  withProgramState(...) would cause vector to be + 1 size -- todo should be a map
		std::vector<int> CurrentState;
		bool ShouldUpdate;

		void registerPlugin(fi::Plugin_Base *Plugin, bool DefaultUpdateValue)
		{
			Plugins.push_back(Plugin);
			Plugin->PluginIndex = (int)Plugins.size() - 1;
			Plugin->ShouldUpdateDefaultValue = DefaultUpdateValue;
			Plugin->ShouldUpdate = DefaultUpdateValue;
		}

		////////////////////////////////////////////////////////////

		void executeOnProgramStart()
		{
			CurrentProgramStateIndex._a.store(-1);
			for (int _i = 0; _i < Plugins.size(); _i++)
			{
				Plugins[_i]->onProgramStart();
			}
		}

		////////////////////////////////////////////////////////////

		bool executeStateChangeIfApplicable()
		{
			if (StateStack.empty())
			{
				fi::log("State stack is empty.  is this an error?");
				return false;
			}

			// ---- Perform change from one state to another if applicable
			if ((CurrentState != StateStack.back()) || (ForceExecuteEnables))
			{
                flagUpdate();

				// ---- save which are currently enabled so that we can properly call onEnableCHange after all is done
				std::vector<bool> PrevState(Plugins.size(), false);
				for (int _i = 0; _i < Plugins.size(); _i++)
				{
					PrevState[_i] = Plugins[_i]->Enabled;
					Plugins[_i]->Enabled = false;
				}


				CurrentState = StateStack.back();
				for (int _i = 0; _i < ProgramStates.size(); _i++)
				{
					if (ProgramStates[_i] == CurrentState)
					{
						CurrentProgramStateIndex._a.store(_i); // todo this is needless work + can be wrong if identical states
					}
				}


				for (int _i = 0; _i < CurrentState.size(); _i++)
				{
					int _Index = CurrentState[_i];
					Plugins[_Index]->Enabled = true;
				}


				// ---- execute onEnableChange(...)
				{
					// ---- execute onEnableChange(false)
					for (int _i = 0; _i < PrevState.size(); _i++)
					{
						if (PrevState[_i] != Plugins[_i]->Enabled)
						{
							if (!Plugins[_i]->Enabled)
							{
								Plugins[_i]->onEnableChange(false);
							}
						}
					}

					// ---- execute onEnableChange(true)
					for (int _i = 0; _i < PrevState.size(); _i++)
					{
						if ((PrevState[_i] != Plugins[_i]->Enabled) || ForceExecuteEnables)
						{
							if (Plugins[_i]->Enabled)
							{
								Plugins[_i]->onEnableChange(true);
							}
						}
					}
				}

                ForceExecuteEnables = false;

				flagUpdate();

				return true;
			}

			return false;
		}

		////////////////////////////////////////////////////////////

		void executeUpdate()
		{
			for (int _i = 0; _i < CurrentState.size(); _i++)
			{
				int _Index = CurrentState[_i];

				Plugins[_Index]->ShouldUpdate = (Plugins[_Index]->ShouldUpdateDefaultValue) || (this->ShouldUpdate);

				if (Plugins[_Index]->ShouldUpdate)
				{
					Plugins[_Index]->onUpdate();
				}

				Plugins[_Index]->ShouldUpdate = Plugins[_Index]->ShouldUpdateDefaultValue;
			}

			this->ShouldUpdate = false;
		}

		////////////////////////////////////////////////////////////

		void executeDraw()
		{
			for (int _i = 0; _i < CurrentState.size(); _i++)
			{
				Plugins[CurrentState[_i]]->onDraw();
			}
		}

		////////////////////////////////////////////////////////////

		void executeResize()
		{
			for (int _i = 0; _i < Plugins.size(); _i++)
			{
				Plugins[_i]->onResize();
			}
		}

		////////////////////////////////////////////////////////////

		void push(std::vector<int> NewActivePlugins)
		{
			StateStack.push_back(NewActivePlugins);
		}

		////////////////////////////////////////////////////////////

		void executeAfterLoad()
		{
			for (int i = 0; i < CurrentState.size(); i++)
			{
				Plugins[i]->afterLoad();
			}
		}

		friend class Engine;
		friend class Program_State_Builder;
	};

    void flagUpdate(); // common to all apps
}

#define FI_MODULE_SERIALIZATION_BOILERPLATE \
void save(cereal::BinaryOutputArchive & ar) override \
{ \
	serialize(ar); \
} \
void load(cereal::BinaryInputArchive & ar) override \
{ \
	serialize(ar); \
} \

#endif