#include "units.h"
#include "nations.h"
#include "vision.h"
#include "map.h"
#include "color_schemes.h"
#include "user_interface.h"

void IUEINW::IUEINW_Plugin_Init_Units::work(const int Event)
{
    getUnits().init();
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Plugin_Draw_Units::work(const int Event)
{
	sf::VertexArray Quads(sf::PrimitiveType::Quads);

	// todo apply column transform

	sf::Color UnderColor = getColorSchemes().getCityUndersideColor();
	int UnitQuadOffset = getColorSchemes().getUnitQuadOffset();
    fi::Slot_Map<IUEINW_Unit> *Units = getUnits().Units.ReadPtr;
    for (int i = 0; i < Units->size(); i++)
    {
    	auto Unit = &Units->Objects[i];
    	sf::Color UnitColor = getNations()[Unit->NationIndex].Color;

		int TickLastSeen = getVision().lastTickSeen(Unit->TileIndex);

		//if (TickLastSeen == getCoreTick()->getTickCount())
		//if (TickLastSeen != -1)
		if (getVision().hasVision(Unit->TileIndex))
		{
			int Alpha = 255;

			// todo

			if (Alpha)
			{
				if (Alpha == 255)
				{
					if (Unit->isSelected() != true)
					{
						getTiles().Grid.buildVertexForCell_Quad(Unit->TileIndex, UnderColor, Quads);
					}
				}

				UnitColor.a = sf::Uint8(Alpha);
				getTiles().Grid.buildVertexForCell_QuadOffset(Unit->TileIndex, UnitColor, UnitQuadOffset, Quads);
			}
		}
    }

    fi::getCanvasWorld().draw(Quads);
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Units::init()
{
    // ---- prototypes
    {
        auto UnitPrototypes = fi::getConfig()["game"]["units"]["types"];

        int Index = 0;
        Prototypes.clear();
        for (auto UnitPrototype : UnitPrototypes)
        {
            Prototypes.push_back(IUEINW_Unit());
            IUEINW_Unit *Unit = &Prototypes.back();
            Unit->Name = UnitPrototype["name"].get<std::string>();
            //Unit->Manpower = UnitPrototype["starting-manpower"].get<int>();
            //Unit->LandCapable = UnitPrototype["land-capable"].get<bool>();
            //Unit->OceanCapable = UnitPrototype["ocean-capable"].get<bool>();
            Unit->VisionRange = UnitPrototype["vision-range"].get<float>();
            Unit->AttackRangeAir = UnitPrototype["attack-range-air"].get<float>();
            Unit->AttackRangeLand = UnitPrototype["attack-range-land"].get<float>();
            Unit->AttackRangeOcean = UnitPrototype["attack-range-ocean"].get<float>();
            //Unit->CanAttack = UnitPrototype["can-attack"].get<bool>();
            //Unit->IsSettler = UnitPrototype["is-settler"].get<bool>();
            //Unit->IsArmy = UnitPrototype["is-army"].get<bool>();
            //Unit->NameTagStr = UnitPrototype["name-tag"].get<std::string>();
            //Unit->CanSufferAttrition = UnitPrototype["can-suffer-attrition"].get<bool>();
            //Unit->TickUpdateInterval = UnitPrototype["tick-update-interval"].get<int>();
            //Unit->UpdatedNTicksAgo = 0;
            Unit->ContinentIndex = -1;
            Unit->PrototypeIndex = Index;
            //Unit->General = 0;
            //Unit->PreviousTileIndex = -1;
            //Unit->InSiege = false;
            Index++;
        }
    }

    // ---- units
    {
        auto WriteAccess = Units.getWriteAccess();
        WriteAccess.Data->clear();
        //CreationRequests.clear();
        //DestructionRequests.clear();
    }
}

////////////////////////////////////////////////////////////

IUEINW::IUEINW_Unit IUEINW::IUEINW_Units::copyUnitPrototype(int PrototypeIndex)
{
    return Prototypes[PrototypeIndex];
}

////////////////////////////////////////////////////////////

IUEINW::IUEINW_Unit IUEINW::IUEINW_Units::copyUnitPrototype(std::string UnitName)
{
    for (auto &Prototype : Prototypes)
    {
        if (Prototype.Name == UnitName)
        {
            return Prototype;
        }
    }

    fi::log("...  Tried to create a unit with a non-existent prototype. Check settings json file. Unit requested:  " + UnitName);
    return IUEINW_Unit();
}

////////////////////////////////////////////////////////////

IUEINW::IUEINW_Unit *IUEINW::IUEINW_Units::createUnit_withRequirements(int NationIndex, int TileIndex,
                                                                       int PrototypeIndex)
{
    // todo subtract values here

    return createUnit_noRequirements(NationIndex, TileIndex, PrototypeIndex);
}

////////////////////////////////////////////////////////////

IUEINW::IUEINW_Unit *IUEINW::IUEINW_Units::createUnit_noRequirements(int NationIndex, int TileIndex, int PrototypeIndex)
{
    auto UnitsWriteAccess = Units.getWriteAccess();
    IUEINW_Unit *Unit = UnitsWriteAccess.Data->createObject();
    fi::Slot_Map_ID ID = Unit->getSlotMapID();
    *Unit = copyUnitPrototype(PrototypeIndex);
    Unit->SlotMapID = ID; // todo copy w/o overwriting id

    // ---- set
    {
        Unit->NationIndex = NationIndex;
        Unit->TickCreated = getCoreTick()->getTickCount();
        Unit->nthUnitInitialized_Nation = ++getNations()[NationIndex].TotalNumberOfUnitsInitialized;

        moveUnitByTileIndex(Unit, TileIndex);
    }

    return Unit;
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Units::destroyUnit(fi::Slot_Map_ID UnitID)
{
    auto UnitsWriteAccess = Units.getWriteAccess();
    auto Unit = UnitsWriteAccess.Data->get(UnitID);

    if (Unit != nullptr)
    {
        UnitsWriteAccess.Data->remove(UnitID);
    }
}

////////////////////////////////////////////////////////////

const bool IUEINW::IUEINW_Units::areSameContinent(const fi::Slot_Map_ID a, const fi::Slot_Map_ID b)
{
    return Units.ReadPtr->get(a)->ContinentIndex == Units.ReadPtr->get(b)->ContinentIndex;
}

////////////////////////////////////////////////////////////

const bool IUEINW::IUEINW_Units::areSameContinent(const IUEINW::IUEINW_Unit *a, const IUEINW::IUEINW_Unit *b)
{
    return a->ContinentIndex == b->ContinentIndex;
}

////////////////////////////////////////////////////////////


fi::Slot_Map<IUEINW::IUEINW_Unit> *IUEINW::IUEINW_Units::getReadPtr()
{
	return Units.ReadPtr;
}

////////////////////////////////////////////////////////////

fi::Lock_Guard_Wrapper<fi::Slot_Map<IUEINW::IUEINW_Unit>> IUEINW::IUEINW_Units::getWriteAccess()
{
    return std::move(Units.getWriteAccess());
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Units::processCreationDestructionRequests(IUEINW::IUEINW_Unit_Creation_Destruction_Request_Lists *Commands)
{
	Commands->process();
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Unit_Creation_Destruction_Request_Lists::process()
{
    std::lock_guard<std::recursive_mutex> lock(CreationDestructionRequestMutex);

    for (int i = 0; i < DestructionRequests.size(); i++)
    {
        getUnits().destroyUnit(DestructionRequests[i]);
    }

    DestructionRequests.clear();

    // ----

    for (int i = 0; i < CreationRequests.size(); i++)
    {
        if (CreationRequests[i].WithRequirements)
        {
			getUnits().createUnit_withRequirements(CreationRequests[i].NationIndex, CreationRequests[i].TileIndex, CreationRequests[i].PrototypeIndex);
        }
        else
        {
			getUnits().createUnit_noRequirements(CreationRequests[i].NationIndex, CreationRequests[i].TileIndex, CreationRequests[i].PrototypeIndex);
        }
    }

    CreationRequests.clear();
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Units:: moveUnitByTileIndex(IUEINW_Unit *UnitWritePtr, int TileIndex)
{
	if (UnitWritePtr->NationIndex == getNations().HumanNationIndex)
	{
		if (getGrid().isValidTile(TileIndex)) // really only possible on spawn
		{
			int CoTSize;
			std::vector<int> *Area = getTiles().getArea(TileIndex, UnitWritePtr->VisionRange, Tile_Type_Grouping::ALL_TILE_TYPES, CoTSize);
			for (int i = 0; i < CoTSize; i++)
			{
				getVision().decrementVision(Area->at(i));
			}
		}
	}

	UnitWritePtr->TileIndex = TileIndex;
	UnitWritePtr->ContinentIndex = getTiles()[TileIndex].ContinentIndex;

	// ---- update vision
	if (UnitWritePtr->NationIndex == getNations().HumanNationIndex)
	{
		int CoTSize;
		std::vector<int> *Area = getTiles().getArea(TileIndex, UnitWritePtr->VisionRange, Tile_Type_Grouping::ALL_TILE_TYPES, CoTSize);
		for (int i = 0; i < CoTSize; i++)
		{
			getVision().revealTile(Area->at(i));
			getVision().incrementVision(Area->at(i));
		}
	}
}

////////////////////////////////////////////////////////////

sf::Vector2f IUEINW::IUEINW_Unit::getTileCenterPosition()
{
	return getGrid().CommonCellData[TileIndex].Center;
}

////////////////////////////////////////////////////////////

bool IUEINW::IUEINW_Unit::isSelected()
{
	return getUI().SelectionManager.isUnitSelected(getSlotMapID());
}

////////////////////////////////////////////////////////////
