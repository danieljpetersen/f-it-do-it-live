#include "plugins.h"
#include "engine.h"

////////////////////////////////////////////////////////////

fi::Program_State_Builder::Program_State_Builder(int ProgramStateIndex)
{
    this->mProgramStateIndex = ProgramStateIndex;

    if (!vectorContains(&fi::getPlugins().ProgramStateIndexesActivePerEachState[mProgramStateIndex], ProgramStateIndex))
    {
        fi::getPlugins().ProgramStateIndexesActivePerEachState[mProgramStateIndex].push_back(ProgramStateIndex);
    }
}

////////////////////////////////////////////////////////////

fi::Program_State_Builder fi::Program_State_Builder::withPlugin(fi::Plugin_Base *Plugin, bool DefaultUpdateValue)
{
	if (Plugin->PluginIndex == -1)
	{
		fi::getPlugins().registerPlugin(Plugin, DefaultUpdateValue);
	}


	if (!vectorContains(&getPlugins().ProgramStates[this->mProgramStateIndex], Plugin->PluginIndex))
	{
		fi::getPlugins().ProgramStates[mProgramStateIndex].push_back(Plugin->PluginIndex);
	}

	return Program_State_Builder(this->mProgramStateIndex);
}

////////////////////////////////////////////////////////////

fi::Program_State_Builder fi::Program_State_Builder::withProgramState(int OtherProgramState)
{
	// you want this here rather than exclusively in the constructor to ensure we catch everything
	auto vec = &fi::getPlugins().ProgramStateIndexesActivePerEachState[OtherProgramState];
	for (int i = 0; i < vec->size(); i++)
	{
		int Other = vec->at(i);
		if (!vectorContains(&fi::getPlugins().ProgramStateIndexesActivePerEachState[mProgramStateIndex], Other))
		{
			fi::getPlugins().ProgramStateIndexesActivePerEachState[mProgramStateIndex].push_back(Other);
		}
	}

	for (int i = 0; i < getPlugins().ProgramStates[OtherProgramState].size(); i++)
	{
		int PluginIndex = getPlugins().ProgramStates[OtherProgramState][i];
		Plugin_Base *Plugin = getPlugins().Plugins[PluginIndex];
		withPlugin(Plugin, Plugin->ShouldUpdateDefaultValue);
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

////////////////////////////////////////////////////////////

fi::Program_State_Builder fi::Program_State_Builder::withTick(std::string TickID)
{
    auto Tick = getTick(TickID);

    Tick->ActiveProgramStates.insert(mProgramStateIndex);

    return fi::Program_State_Builder(this->mProgramStateIndex);
}

////////////////////////////////////////////////////////////

void fi::flagUpdate()
{
    getPlugins().flagUpdate();
}
