#include "../../engine.h"
#include "gui_tick_controller.h"

////////////////////////////////////////////////////////////

void fi::Tick_Controller::update(fi::Tick *Tick)
{
	const int InitialTickSpeed = SelectedTickSpeed;

	if (fi::getInput().check("Tick Speed 0")) // aka pause
	{
		if (SelectedTickSpeed == 0)
		{
			SelectedTickSpeed = TickSpeedPriorToPause;
		}
		else
		{
			TickSpeedPriorToPause = SelectedTickSpeed;
			SelectedTickSpeed = 0;
		}

		fi::getGUI().setFocus(-1);
	}

	std::vector<std::string> Labels { "-" };
	for (int i = 0; i < Tick->TicksPerSecondOptions.size(); i++)
	{
		if (fi::getInput().check("Tick Speed " + fi::to_s(i + 1)))
		{
			SelectedTickSpeed = i + 1;
			fi::getGUI().setFocus(-1);
		}

		std::string Label = "";
		for (int j = 0; j < i+1; j++)
		{
			Label += ">";
		}
		Labels.push_back(Label);
	}


	// ---- speed buttons
	fi::guiButtonsHorizontal(new fi::GUI_Node(fi::ANCHOR_START, fi::ANCHOR_START_ABSOLUTE, xConstraintMinWidth(45), yConstraintNormalized(1.0f)), Labels, SelectedTickSpeed);

	if (SelectedTickSpeed == 0)
	{
		if (Tick->isPaused() != true)
		{
			Tick->pauseTimestep();
		}
	}
	else if (InitialTickSpeed != SelectedTickSpeed)
	{
		double TicksPerSecond = Tick->TicksPerSecondOptions[SelectedTickSpeed-1];
		Tick->swapTimestep(TicksPerSecond);

		if (Tick->isPaused())
		{
			Tick->unpauseTimestep();
		}
	}

	// ---- Tick Count Box
	{
		fi::GUI_Node *Node = new fi::GUI_Node(fi::ANCHOR_START, fi::ANCHOR_START_ABSOLUTE, xConstraintMinWidth(77), yConstraintNormalized(1.0f));
		if (ShowingProgressBarForTickCounter)
		{
			if (fi::guiProgressBar(Node, (float)Tick->getInterpolation(), fi::to_s(Tick->getTickCount())))
			{
				ShowingProgressBarForTickCounter = !ShowingProgressBarForTickCounter;
			}
		}
		else
		{
			if (fi::guiButton(Node, fi::to_s(Tick->getTickCount())))
			{
				ShowingProgressBarForTickCounter = !ShowingProgressBarForTickCounter;
			}
		}
	}
}

////////////////////////////////////////////////////////////
