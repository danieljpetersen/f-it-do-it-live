#ifndef FI_PLUGINS_H
#define FI_PLUGINS_H

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <deque>
#include "atomic_wrapper.h"
#include "cereal/archives/binary.hpp"
#include "non_movable.h"
#include "non_copyable.h"
#include "util.h"

namespace fi
{
    const int EVENT_PRE_LOAD = 1;
    const int EVENT_SERIALIZE = 0;
    const int EVENT_POST_LOAD = 2;
    const int EVENT_PRE_UPDATE = 3;
    const int EVENT_UPDATE = 4;
    const int EVENT_POST_UPDATE = 5;
    const int EVENT_DRAW = 6;
    const int EVENT_RESIZE = 7;
    const int FI_EVENT_COUNT = 8;

    ////////////////////////////////////////////////////////////
    // a wrapper for an empty overridable function which is automatically called at specified events
    class Plugin_Base
    {
    public:
        int PluginIndex = -1;
        bool Enabled = false;

        virtual void work(const int Event) {};
        virtual void onEnableChange(bool Enabling) {};
    };

    ////////////////////////////////////////////////////////////
    // ProgramState is a set of plugins eligible to execute
    class Program_State_Builder
    {
    public:
        Program_State_Builder(int ProgramStateIndex);
        Program_State_Builder withPlugin(Plugin_Base *Plugin, int OnEvent);
        Program_State_Builder withTick(int TickID);
        Program_State_Builder withTick(std::string TickID);
        Program_State_Builder withProgramState(int OtherProgramState);

    private:
        int mProgramStateIndex = -1;
    };

    ////////////////////////////////////////////////////////////

    class Plugin_Manager : public Non_Copyable, public Non_Movable
    {
    public:
        Program_State_Builder defineProgramState(int &ProgramStateIndex)
        {
            ProgramStateIndex = (signed)EnabledPluginsPerStatePerEventType.size();
            EnabledPluginsPerStatePerEventType.resize(EnabledPluginsPerStatePerEventType.size() + 1);

            ProgramStatesPerProgramState.resize(ProgramStatesPerProgramState.size()+1);
            ProgramStatesPerProgramState.back().push_back(ProgramStateIndex);

            return Program_State_Builder(ProgramStateIndex);
        }

        // ProgramStates used to be but are no longer a stack, as I never actually used it as a stack.
        // setting a new ProgramState will replace the current ProgramState (as well as any previously queued ProgramState) at the start of new frames
        // so the current set of executing Plugins will always finish executing if a state change is performed, and again, next plugin set starts next frame
        void setProgramState(int ProgramStateIndex)
        {
            NextProgramState = ProgramStateIndex;

            // first time being set, let's call the relevant onEnabled
            if (CurrentProgramStateIndex == -1)
            {
                changeStateIfApplicable();
            }
        }

        // two functions for this because, thanks to ProgramStateBuilder.withProgramState(...), a ProgramState can made up of multiple ProgramStates.
        // thus isProgramStateEnabled returns true if, for example, the program state you are querying was made with a call to withProgramState(QueriedProgramState)
        bool isProgramStateEnabled(int ProgramStateIndex)
        {
            if (CurrentProgramStateIndex == ProgramStateIndex)
            {
                return true;
            }

            // ie:  withProgramState(...) was used.
            if (CurrentProgramStateIndex == -1)
            {
                return false;
            }

            for (int i = 0; i < ProgramStatesPerProgramState[CurrentProgramStateIndex].size(); i++)
            {
                if (ProgramStatesPerProgramState[CurrentProgramStateIndex][i] == ProgramStateIndex)
                {
                    return true;
                }
            }

            return false;
        }

        int getCurrentProgramStateIndex()
        {
            return CurrentProgramStateIndex;
        }

        // engine calls this->execute(...) for all the default EVENT_* events, but users are equally able to call execute with user defined events
        // note that executions happen immediately, so if you're in the middle of an update event, plugins which are yet to be executed for update will wait
        // until the new event is done executing
        void execute(int EventType)
        {
            if (CurrentProgramStateIndex == -1)
            {
                return;
            }

            std::vector<int> *PluginsToExecute = &EnabledPluginsPerStatePerEventType[CurrentProgramStateIndex][EventType];
            for (int i = 0; i < PluginsToExecute->size(); i++)
            {
                int PluginIndex = PluginsToExecute->at(i);
                Plugins[PluginIndex]->work(EventType);
            }

            if (DelayedExecutes.empty() != true)
            {
                auto Event = DelayedExecutes.front();

                // second being the Event that the delayed event should execute after. -1 for after the current stack is exhausted
                if ((Event.second == -1) || (Event.second == EventType))
                {
                    DelayedExecutes.pop_front();
                    execute(Event.first);
                }
            }
        }

        // same as execute but can queue events; will wait until the current event (+ any previous delayedExecute) is exhausted
        void delayedExecute(int EventType)
        {
            // not intuitive but i don't want the events to be repeatable. if an event is queued, queuing it again before firing won't do anything
            if (isEventQueued(EventType))
            {
                return;
            }

            DelayedExecutes.push_back(std::pair<int, int>(EventType, -1));
        }

        void delayedExecute(int EventType, int EventTypeToExecuteAfter)
        {
            // not intuitive but i don't want the events to be repeatable. if an event is queued, queuing it again before firing won't do anything
            if (isEventQueued(EventType))
            {
                return;
            }

            DelayedExecutes.push_back(std::pair<int, int>(EventType, EventTypeToExecuteAfter));
        }

        bool isEventQueued(int EventType)
        {
            for (int i = 0; i < DelayedExecutes.size(); i++)
            {
                if (DelayedExecutes[i].first == EventType)
                {
                    return true;
                }
            }

            return false;
        }

    private:
        std::vector<fi::Plugin_Base*> Plugins;

        // slightly confusing construct here but basically for each program state we have a map with a key of event type, value of vector<int>.
        // vector<int> corresponds to plugins called for that event.  So:
        // first dimension: int ProgramStateIndex
        // map key: int EventType
        // third dimension: the set (duplicates not allowed, ordered according to the order of withPlugin(...) calls) of enabled plugins for that state / event combination
        std::vector<std::unordered_map<int, std::vector<int>>> EnabledPluginsPerStatePerEventType;

        std::vector<std::vector<int>> ProgramStatesPerProgramState; // like was said, ProgramStates can be made up of multiple ProgramStates. This tracks that

        std::deque<std::pair<int, int>> DelayedExecutes; // EventType, EventTypeToExecuteAfter (-1 for after current event is exhuasted)

        int CurrentProgramStateIndex = -1;
        int NextProgramState = -1;

        bool changeStateIfApplicable()
        {
            if ((CurrentProgramStateIndex == -1) && (NextProgramState == -1))
            {
                fi::log("State never set. is this an error?");
            }

            if (NextProgramState != -1)
            {
                // ---- save which are currently enabled so that we can properly call onEnableChange after all is done
                std::vector<bool> PrevState(Plugins.size(), false);
                for (int _i = 0; _i < Plugins.size(); _i++)
                {
                    PrevState[_i] = Plugins[_i]->Enabled;
                    Plugins[_i]->Enabled = false;
                }

                // ---- mark the relevant plugins as enabled
                for (int _i = 0; _i < EnabledPluginsPerStatePerEventType[NextProgramState].size(); _i++)
                {
                    for (auto Events : EnabledPluginsPerStatePerEventType[NextProgramState])
                    {
                        for (int j = 0; j < Events.second.size(); j++)
                        {
                            int _Index = Events.second[j];
                            Plugins[_Index]->Enabled = true;
                        }
                    }

                }

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
                    if (PrevState[_i] != Plugins[_i]->Enabled)
                    {
                        if (Plugins[_i]->Enabled)
                        {
                            Plugins[_i]->onEnableChange(true);
                        }
                    }
                }

                CurrentProgramStateIndex = NextProgramState;
                NextProgramState = -1;
            }

            return false;
        }

        void registerPlugin(fi::Plugin_Base *Plugin)
        {
            Plugins.push_back(Plugin);
            Plugin->PluginIndex = (int)Plugins.size() - 1;
        }

        friend class Program_State_Builder;
        friend class Engine;
    };
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