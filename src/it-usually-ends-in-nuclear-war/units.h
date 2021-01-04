#ifndef IUEINW_UNITS_H
#define IUEINW_UNITS_H

#include "../common/engine.h"

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
        int TileIndex, NationIndex, ContinentIndex, PrototypeIndex, TickCreated;
        int VisionRange, AttackRangeAir, AttackRangeLand, AttackRangeOcean;
        int nthUnitInitialized_Nation;
        float TransparencyToHuman;
        sf::Color Color;
    };

    ////////////////////////////////////////////////////////////

    class IUEINW_Units
    {
    public:
        fi::Slot_Map<IUEINW_Unit> Units;
        std::vector<IUEINW_Unit> Prototypes;

        IUEINW_Unit   operator [](int i) const {return Units[i];}
        IUEINW_Unit & operator [](int i)       {return Units[i];}

        void init();

        IUEINW_Unit* spawnUnit_withRequirements(int NationIndex, int TileIndex, int PrototypeIndex);
        IUEINW_Unit* spawnUnit_noRequirements(int NationIndex, int TileIndex, int PrototypeIndex);
        void destroyUnit(fi::Slot_Map_ID UnitID);

        const bool areSameContinent(const fi::Slot_Map_ID a, const fi::Slot_Map_ID b);
        const bool areSameContinent(const IUEINW_Unit *a, const IUEINW_Unit *b);

    private:
        IUEINW_Unit copyUnitPrototype(int PrototypeIndex);
        IUEINW_Unit copyUnitPrototype(std::string UnitName);
    };

    IUEINW_Units &getUnits();
}

#endif