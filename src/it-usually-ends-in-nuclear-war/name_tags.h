#ifndef IUEINW_NAME_TAGS
#define IUEINW_NAME_TAGS

#include <vector>
#include "../common/plugins.h"

namespace IUEINW
{
    class IUEINW_Name_Tag
    {
    public:
        int Strength;
        bool HasCity = false;
    };

    // ----

    class IUEINW_Plugin_Name_Tags : public fi::Plugin_Base
    {
    public:
        std::unordered_map<int, IUEINW_Name_Tag> NameTags; // key==TileIndex;

        void work(const int Event)
        {
            NameTags.clear();
            addCityNameTags();
            addUnitNameTags();
            renderNameTags();
        }

    private:
        void addUnitNameTags();
        void addCityNameTags();
        void renderNameTags();
    };
}

#endif