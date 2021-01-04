#ifndef IUEINW_MAP_SIZE_H
#define IUEINW_MAP_SIZE_H

#include <string>
#include <vector>
#include "../common/engine.h"
#include "globals.h"

namespace IUEINW
{
    class IUEINW_Map_Layout
    {
    private:
        std::string Name;
        int SmallToLargeConversionScale;
        int SmallRowCount, SmallColumnCount;

    public:
        int SuggestedNumberOfNations;
        int StartingNumberOfNations;
        int LargeRowCount, LargeColumnCount;
        int MinimumNationSpawnDistance;

        ////////////////////////////////////////////////////////////

        template <class Archive>
        void serialize( Archive & ar )
        {
            ar(SuggestedNumberOfNations);
            ar(StartingNumberOfNations);
            ar(LargeRowCount);
            ar(LargeColumnCount);
            ar(MinimumNationSpawnDistance);

            ar(SmallRowCount);
            ar(SmallColumnCount);
            ar(SmallToLargeConversionScale);
            ar(Name);
        }

        ////////////////////////////////////////////////////////////

        void init(std::string Name, int SmallToLargeConversionScale, int SmallRowCount, int SmallColumnCount, int SuggestedNumberOfNations, int MinimumNationSpawnDistance)
        {
            this->Name = Name;
            this->SmallToLargeConversionScale = SmallToLargeConversionScale;
            this->SmallRowCount = SmallRowCount;
            this->SmallColumnCount = SmallColumnCount;
            this->SuggestedNumberOfNations = SuggestedNumberOfNations;
            this->MinimumNationSpawnDistance = MinimumNationSpawnDistance;

            LargeColumnCount = SmallToLargeConversionScale * SmallColumnCount;
            LargeRowCount = SmallToLargeConversionScale * SmallRowCount;
        }

        ////////////////////////////////////////////////////////////

        std::string getName()
        {
            return Name;
        }

        ////////////////////////////////////////////////////////////

        int getScale()
        {
            return SmallToLargeConversionScale;
        }

        ////////////////////////////////////////////////////////////

        int getSmallRows()
        {
            return SmallRowCount;
        }

        ////////////////////////////////////////////////////////////

        int getSmallCols()
        {
            return SmallColumnCount;
        }
    };

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

    class IUEINW_Map_Layouts
    {
    public:
        std::vector<IUEINW_Map_Layout> Objects;
        IUEINW_Map_Layout   operator [](int i) const {return Objects[i];}
        IUEINW_Map_Layout & operator [](int i)       {return Objects[i];}

        ////////////////////////////////////////////////////////////


        template <class Archive>
        void serialize( Archive & ar )
        {
            ar(Objects);
        }

        ////////////////////////////////////////////////////////////

        void initFromJSON()
        {
            Objects.clear();

            auto array = fi::getConfig()["game"]["map"]["sizes"];

            for (auto it = array.begin(); it != array.end(); ++it)
            {
                std::string Name = (*it)["name"].get<std::string>();
                int Scale = (*it)["scale"].get<int>();
                int SmallRows = (*it)["rows"].get<int>();
                int SmallCols = (*it)["cols"].get<int>();
                int SuggestedNationCount = (*it)["suggested-number-of-nations"].get<int>();
                int MinimumNationSpawnDistance = (*it)["minimum-nation-spawn-distance"].get<int>();

                IUEINW_Map_Layout Size;
                Size.init(Name, Scale, SmallRows, SmallCols, SuggestedNationCount, MinimumNationSpawnDistance);
                Objects.push_back(Size);
            }

            CITY_RING_SIZE = fi::getConfig()["game"]["city-ring-size"].get<int>();
        }

        ////////////////////////////////////////////////////////////

        int size()
        {
            return (int)Objects.size();
        };

        ////////////////////////////////////////////////////////////

        IUEINW_Map_Layout getSuggestedMapSize()
        {
            for (unsigned int i = 0; i < Objects.size(); i++)
            {
                if (Objects[i].getName() == "normal")
                    return Objects[i];
            }

            return Objects.front();
        }
    };
}

#endif
