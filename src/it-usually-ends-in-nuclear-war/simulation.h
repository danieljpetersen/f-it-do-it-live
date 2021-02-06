#ifndef IUEINW_SIMULATION_H
#define IUEINW_SIMULATION_H

#include "../common/engine.h"
#include "units.h"
#include "pathfinding.h"

namespace IUEINW
{
    class IUEINW_Simulation_Input
    {
    public:
        IUEINW_Simulation_Input() = default;
		IUEINW_Unit_Creation_Destruction_Request_Lists UnitCreationDestructionRequests;
		std::vector<IUEINW_Path> Paths;
	};

	// ----

    class IUEINW_Plugin_Simulation : public fi::Plugin_Base, public fi::Work_Queue<std::shared_ptr<IUEINW_Simulation_Input>>
    {
    public:
		IUEINW_Plugin_Simulation()
		{
			NextSimulationInput = new IUEINW_Simulation_Input();
		}

        std::shared_ptr<IUEINW_Simulation_Input> SimulationInput;
        IUEINW_Simulation_Input *NextSimulationInput;

        void work(const int EventID) override
        {
            if (getCoreTick()->isStepThisFrame())
            {
                addWork(std::shared_ptr<IUEINW_Simulation_Input>(NextSimulationInput));

                NextSimulationInput = new IUEINW_Simulation_Input();

                waitForAllWorkComplete_Help();

                fi::Double_Buffer_Object_Tracker::instance()._swapPointers();
            }
        }

        // theory:  it is the ai that matters.
        //          calculating the next iteration of the state should be very quick.
        //          design it to be quick. ie: no pathfinding here. it's just going through a loop of like aUnit->TileIndex = Path->pop_front();
        //          the bulk processing should be ai.plan();
        //          ai.plan would be launched immediately
        //          here too there would be no discrepancy between player input and ai input. networked players can repeat the same process
        //          in other words, the IUEINW_Simulation_Input would be sent as a packet

        void asyncWork(std::shared_ptr<IUEINW_Simulation_Input> SimulationInput) override
        {
            this->SimulationInput = SimulationInput;

            fi::Double_Buffer_Object_Tracker::instance()._copyData(); // todo each double buffer should get it's own background job; should add work and wait/help here for completion

            getUnits().processCreationDestructionRequests(&SimulationInput->UnitCreationDestructionRequests);

			auto UnitsWriteAccess = getUnits().getWriteAccess();
			auto UnitsWritePtr = UnitsWriteAccess.Data;

			for (int i = 0; i < SimulationInput->Paths.size(); i++)
			{
            	auto Unit = UnitsWritePtr->get(SimulationInput->Paths[i].UnitID);
            	Unit->Path = SimulationInput->Paths[i];
			}

			for (int i = 0; i < UnitsWritePtr->size(); i++)
			{
				IUEINW_Unit *Unit = &UnitsWritePtr->Objects[i];

				if (Unit != nullptr)
				{
					if (Unit->Path.Path != nullptr)
					{
						if (Unit->Path.Path->size())
						{
							Unit->TileIndex = Unit->Path.Path->front();
							Unit->Path.Path->pop_front();
						}
						else
						{
							Unit->Path.Path = nullptr;
						}
					}
				}
			}
            // todo -- think about passinng write access to the functions that need them; 2 versions of the func; one which creates write access, and one which gets passed

            // wait for work to complete here
        }
    };

    // ----

    IUEINW_Plugin_Simulation &getSimulation();
}

#endif