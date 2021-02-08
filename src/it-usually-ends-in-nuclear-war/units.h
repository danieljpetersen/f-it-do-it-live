#ifndef IUEINW_UNITS_H
#define IUEINW_UNITS_H

#include "../common/engine.h"
#include "path.h"

namespace IUEINW
{
    class IUEINW_Plugin_Init_Units : public fi::Plugin_Base
    {
    public:
        void work(const int Event) override;
    };

    ////////////////////////////////////////////////////////////

    class IUEINW_Plugin_Draw_Units : public fi::Plugin_Base
    {
    public:
        void work(const int Event) override;
    };

    ////////////////////////////////////////////////////////////

    class IUEINW_Unit : public fi::Slot_Map_Item
    {
    public:
        std::string Name;
        int Veterancy = 0;
        int Strength = 1;

        int TileIndex, NationIndex, ContinentIndex, PrototypeIndex, TickCreated;
        int VisionRange, AttackRangeAir, AttackRangeLand, AttackRangeOcean;
        int nthUnitInitialized_Nation;

        IUEINW_Path Path;
        sf::Vector2f getTileCenterPosition();
        bool isSelected();
	};

    ////////////////////////////////////////////////////////////

    class IUEINW_Unit_Creation_Request
    {
    public:
        IUEINW_Unit_Creation_Request() = default;

        IUEINW_Unit_Creation_Request(int NationIndex, int TileIndex, int PrototypeIndex, bool WithRequirements)
        {
            this->NationIndex = NationIndex;
            this->TileIndex = TileIndex;
            this->PrototypeIndex = PrototypeIndex;
            this->WithRequirements = WithRequirements;
        }

        int NationIndex, TileIndex, PrototypeIndex;
        bool WithRequirements;
    };

    ////////////////////////////////////////////////////////////

    class IUEINW_Unit_Creation_Destruction_Request_Lists
    {
    public:
        void process();

        // ----

        void unitCreationRequest(int NationIndex, int TileIndex, int PrototypeIndex, bool WithRequirements)
        {
            std::lock_guard<std::recursive_mutex> lock(CreationDestructionRequestMutex);

            IUEINW_Unit_Creation_Request CreationRequest;
            CreationRequest.NationIndex = NationIndex;
            CreationRequest.TileIndex = TileIndex;
            CreationRequest.PrototypeIndex = PrototypeIndex;
            CreationRequest.WithRequirements = WithRequirements;
            CreationRequests.push_back(CreationRequest);
        }

        void unitDestructionRequest(fi::Slot_Map_ID ID)
        {
            std::lock_guard<std::recursive_mutex> lock(CreationDestructionRequestMutex);
            DestructionRequests.push_back(ID);
        }

    private:
        std::recursive_mutex CreationDestructionRequestMutex;
        std::vector<IUEINW_Unit_Creation_Request> CreationRequests;
        std::vector<fi::Slot_Map_ID> DestructionRequests;
    };

    ////////////////////////////////////////////////////////////

    class IUEINW_Units
    {
    public:
        std::vector<IUEINW_Unit> Prototypes;
        fi::Double_Buffer<fi::Slot_Map<IUEINW_Unit>> Units;

        void init();
		fi::Slot_Map<IUEINW_Unit> *getReadPtr();
		fi::Lock_Guard_Wrapper<fi::Slot_Map<IUEINW_Unit>> getWriteAccess();
		void processCreationDestructionRequests(IUEINW_Unit_Creation_Destruction_Request_Lists *Commands);

		int prototypeNameToIndex(std::string UnitName);

		const bool areSameContinent(const fi::Slot_Map_ID a, const fi::Slot_Map_ID b);
        const bool areSameContinent(const IUEINW_Unit *a, const IUEINW_Unit *b);
		void moveUnitByTileIndex(IUEINW_Unit *UnitWritePtr, int TileIndex);


	private:
        IUEINW_Unit copyUnitPrototype(int PrototypeIndex);
        IUEINW_Unit copyUnitPrototype(std::string UnitName);

        IUEINW_Unit* createUnit_withRequirements(int NationIndex, int TileIndex, int PrototypeIndex);
        IUEINW_Unit* createUnit_noRequirements(int NationIndex, int TileIndex, int PrototypeIndex);
        void destroyUnit(fi::Slot_Map_ID UnitID);

		friend class IUEINW_Unit_Creation_Destruction_Request_Lists;
    };

    IUEINW_Units &getUnits();
}

#endif