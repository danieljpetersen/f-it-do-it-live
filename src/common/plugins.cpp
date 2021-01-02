#include "plugins.h"
#include "engine.h"

////////////////////////////////////////////////////////////

fi::Program_State_Builder::Program_State_Builder(int ProgramStateIndex)
{
    this->mProgramStateIndex = ProgramStateIndex;

    if (!vectorContains(&fi::getPlugins().ProgramStatesPerProgramState[mProgramStateIndex], ProgramStateIndex))
    {
        fi::getPlugins().ProgramStatesPerProgramState[mProgramStateIndex].push_back(ProgramStateIndex);
    }
}

////////////////////////////////////////////////////////////

fi::Program_State_Builder fi::Program_State_Builder::withPlugin(fi::Plugin_Base *Plugin, int OnEvent)
{
    if (Plugin->PluginIndex == -1)
    {
        fi::getPlugins().registerPlugin(Plugin);
    }

    std::vector<int> *EventPlugins = &getPlugins().EnabledPluginsPerStatePerEventType[mProgramStateIndex][OnEvent];

    if (!vectorContains(EventPlugins, Plugin->PluginIndex))
    {
        EventPlugins->push_back(Plugin->PluginIndex);
    }

    return Program_State_Builder(this->mProgramStateIndex);
}

////////////////////////////////////////////////////////////

fi::Program_State_Builder fi::Program_State_Builder::withTick(int TickID)
{
    auto Tick = getTick(TickID);

    Tick->ActiveProgramStates.insert(mProgramStateIndex);

    return fi::Program_State_Builder(this->mProgramStateIndex);
}

////////////////////////////////////////////////////////////

fi::Program_State_Builder fi::Program_State_Builder::withTick(std::string TickID)
{
    auto Tick = getTick(TickID);

    Tick->ActiveProgramStates.insert(mProgramStateIndex);

    return fi::Program_State_Builder(this->mProgramStateIndex);
}

////////////////////////////////////////////////////////////

fi::Program_State_Builder fi::Program_State_Builder::withProgramState(int OtherProgramState)
{
    auto vec = &fi::getPlugins().ProgramStatesPerProgramState[OtherProgramState];
    for (int i = 0; i < vec->size(); i++)
    {
        int Other = vec->at(i);
        if (!vectorContains(&fi::getPlugins().ProgramStatesPerProgramState[mProgramStateIndex], Other))
        {
            fi::getPlugins().ProgramStatesPerProgramState[mProgramStateIndex].push_back(Other);
        }
    }

    for (int i = 0; i < getPlugins().EnabledPluginsPerStatePerEventType[OtherProgramState].size(); i++)
    {
        std::unordered_map<int, std::vector<int>> *MapEventsToPlugins = &getPlugins().EnabledPluginsPerStatePerEventType[OtherProgramState];

        for (auto EventPlugins : *MapEventsToPlugins)
        {
            for (int j = 0; j < EventPlugins.second.size(); j++)
            {
                int PluginIndex = EventPlugins.second[j];
                Plugin_Base *Plugin = getPlugins().Plugins[PluginIndex];
                withPlugin(Plugin, EventPlugins.first);
            }
        }
    }

    for (int i = 0; i < fi::getEngine().Ticks.size(); i++)
    {
        auto Tick = &fi::getEngine().Ticks[i];
        if (Tick->isActiveDuringProgramState(OtherProgramState))
        {
            Tick->ActiveProgramStates.insert(mProgramStateIndex);
        }
    }

    return Program_State_Builder(this->mProgramStateIndex);
}
