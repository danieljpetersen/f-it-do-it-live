#include "units.h"
#include "nations.h"
#include "vision.h"
#include "map.h"
#include "color_schemes.h"

void IUEINW::IUEINW_Plugin_Init_Units::work(const int Event)
{
    getUnits().init();
}


////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Plugin_Draw_Units::work(const int Event)
{
    sf::VertexArray Quads(sf::PrimitiveType::Quads);

    int UnitQuadOffset = getColorSchemes().getUnitQuadOffset();
    fi::Slot_Map<IUEINW_Unit> *Units = &getUnits().Units;
    for (int i = 0; i < Units->size(); i++)
    {
        if (getVision().hasVision(Units->Objects[i].TileIndex))
        {
            getTiles().Grid.buildVertexForCell_QuadOffset(Units->Objects[i].TileIndex, Units->Objects[i].Color, UnitQuadOffset, Quads);
        }
    }

    fi::getCanvasWorld().draw(Quads);
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Units::init()
{
    // ---- create prototypes
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

IUEINW::IUEINW_Unit *IUEINW::IUEINW_Units::spawnUnit_withRequirements(int NationIndex, int TileIndex, int PrototypeIndex)
{
    // todo subtract values here

    return spawnUnit_noRequirements(NationIndex, TileIndex, PrototypeIndex);
}

////////////////////////////////////////////////////////////

IUEINW::IUEINW_Unit *IUEINW::IUEINW_Units::spawnUnit_noRequirements(int NationIndex, int TileIndex, int PrototypeIndex)
{
    IUEINW_Unit *Unit = Units.createObject();
    fi::Slot_Map_ID ID = Unit->getSlotMapID();
    *Unit = copyUnitPrototype(PrototypeIndex);
    Unit->SlotMapID = ID; // todo copy w/o overwriting id

    // ---- set Properties
    {
        Unit->NationIndex = NationIndex;
        Unit->TickCreated = getCoreTick()->getTickCount();
        Unit->TileIndex = TileIndex;
        Unit->ContinentIndex = getTiles()[TileIndex].ContinentIndex;
        Unit->TransparencyToHuman = 0.0f;
        Unit->nthUnitInitialized_Nation = ++getNations()[NationIndex].TotalNumberOfUnitsInitialized;
        Unit->Color = getNations()[NationIndex].Color;
    }

    getNations()[NationIndex].Units.push_back(Unit->getSlotMapID());
    //App.Tiles.Grid.CustomCellData[TileIndex].addUnit(Unit);
    //Unit->setAction_AutoExplore(-1, -1, false);

    return Unit;
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Units::destroyUnit(fi::Slot_Map_ID UnitID)
{
    auto Unit = Units.get(UnitID);

    if (Unit != nullptr)
    {
        // ---- remove from nation
        {
            int NationIndex = Unit->NationIndex;
            std::vector<fi::Slot_Map_ID> *Vec = &getNations()[NationIndex].Units;
            Vec->erase(std::remove(Vec->begin(), Vec->end(), UnitID), Vec->end());
        }

        Units.remove(UnitID);
    }
}

////////////////////////////////////////////////////////////

const bool IUEINW::IUEINW_Units::areSameContinent(const fi::Slot_Map_ID a, const fi::Slot_Map_ID b)
{
    return Units.get(a)->ContinentIndex == Units.get(b)->ContinentIndex;
}

////////////////////////////////////////////////////////////

const bool IUEINW::IUEINW_Units::areSameContinent(const IUEINW::IUEINW_Unit *a, const IUEINW::IUEINW_Unit *b)
{
    return a->ContinentIndex == b->ContinentIndex;
}

////////////////////////////////////////////////////////////

