
#include "map_generation.h"
#include "color_schemes.h"

void IUEINW::IUEINW_Plugin_Generate_Map::largeBoard_determineTileShadows()
{
    for (int i = 0; i < getTiles().size(); i++)
    {
        largeBoard_determineTileShadow(i);
    }
}

////////////////////////////////////////////////////////////

void IUEINW::IUEINW_Plugin_Generate_Map::largeBoard_determineTileShadow(int Index)
{
    int Height[4];
    Height[LEFT] = getTiles()[Index].Height[LEFT];
    Height[RIGHT] = getTiles()[Index].Height[RIGHT];
    Height[BACK] = getTiles()[Index].Height[BACK];
    Height[FRONT] = getTiles()[Index].Height[FRONT];

    getTiles()[Index].HorizontalSplit = false;
    getTiles()[Index].VerticalSplit = false;

    sf::Color FlatLandColor = getTiles()[Index].aColor;
    sf::Color Lighter = getColorSchemes().calculateLighterColor_Shadow(FlatLandColor);
    sf::Color Darker = getColorSchemes().calculateDarkerColor_Shadow(FlatLandColor);

    //======================================================================

    if ((Height[LEFT] == Height[RIGHT]) && (Height[LEFT] == Height[BACK]) && (Height[LEFT] == Height[FRONT]))
    {
        getTiles()[Index].aColor = FlatLandColor;
        return;
    }

    //===========
    if ((Height[RIGHT] == Height[BACK]) && (Height[RIGHT] == Height[FRONT]) && (Height[LEFT] != Height[RIGHT]))
    {
        getTiles()[Index].VerticalSplit = true;

        if (Height[LEFT] > Height[RIGHT])
        {
            getTiles()[Index].aColor =Lighter;
            getTiles()[Index].bColor = FlatLandColor;
        }
        else
        {

            getTiles()[Index].aColor = Darker;
            getTiles()[Index].bColor = FlatLandColor;
        }

        return;
    }

    if ((Height[LEFT] == Height[BACK]) && (Height[LEFT] == Height[FRONT]) && (Height[LEFT] != Height[RIGHT]))
    {
        getTiles()[Index].VerticalSplit = true;

        if (Height[RIGHT] > Height[LEFT])
        {
            getTiles()[Index].aColor = FlatLandColor;
            getTiles()[Index].bColor = Darker;
        }
        else
        {
            getTiles()[Index].aColor = FlatLandColor;
            getTiles()[Index].bColor = Lighter;
        }

        return;
    }

    if ((Height[LEFT] == Height[BACK]) && (Height[LEFT] == Height[RIGHT]) && (Height[LEFT] != Height[FRONT]))
    {
        getTiles()[Index].HorizontalSplit = true;

        if (Height[FRONT] > Height[LEFT])
        {
            getTiles()[Index].aColor = FlatLandColor;
            getTiles()[Index].bColor = Darker;
        }
        else
        {
            getTiles()[Index].aColor = FlatLandColor;
            getTiles()[Index].bColor = Lighter;
        }

        return;
    }

    if ((Height[LEFT] == Height[FRONT]) && (Height[LEFT] == Height[RIGHT]) && (Height[LEFT] != Height[BACK]))
    {
        getTiles()[Index].HorizontalSplit = true;

        if (Height[BACK] > Height[LEFT])
        {
            getTiles()[Index].aColor = Lighter;
            getTiles()[Index].bColor = FlatLandColor;//was backfront instead of flatland wtf???
        }
        else
        {
            getTiles()[Index].aColor = Darker;
            getTiles()[Index].bColor = FlatLandColor;//was backfront instead of flatland wtf???
        }

        return;
    }

    //=======
    if ((Height[LEFT] == Height[BACK]) && (Height[RIGHT] == Height[FRONT]) && (Height[LEFT] > Height[FRONT]))
    {
        getTiles()[Index].aColor = Lighter;
        return;
    }

    if ((Height[LEFT] == Height[FRONT]) && (Height[BACK] == Height[RIGHT]) && (Height[FRONT] > Height[BACK]))
    {
        getTiles()[Index].aColor = Darker;
        return;
    }
    if ((Height[LEFT] == Height[FRONT]) && (Height[BACK] == Height[RIGHT]) && (Height[FRONT] < Height[BACK]))
    {
        getTiles()[Index].aColor = Darker;
        return;
    }
    //=============
    if ((Height[BACK] == Height[FRONT]) && (Height[LEFT] == Height[RIGHT]))
    {
        if (Height[BACK] != Height[LEFT])
        {
            if (Height[BACK] > Height[LEFT])
            {
                getTiles()[Index].VerticalSplit = true;
                getTiles()[Index].aColor = Darker;
                getTiles()[Index].bColor = Lighter;
            }
            else
            {
                getTiles()[Index].HorizontalSplit = true;
                getTiles()[Index].aColor = Darker;
                getTiles()[Index].bColor = Lighter;
            }
            return;
        }
    }

    //-----
    if ((Height[BACK] + Height[FRONT] + Height[LEFT]) > ((Height[BACK] + Height[FRONT] + Height[RIGHT])))
    {
        getTiles()[Index].aColor = Lighter;
        return;
    }
    if ((Height[BACK] + Height[FRONT] + Height[LEFT]) < ((Height[BACK] + Height[FRONT] + Height[RIGHT])))
    {
        getTiles()[Index].aColor = Darker;
        return;
    }
    if ((Height[LEFT] + Height[BACK] + Height[RIGHT]) > ((Height[LEFT] + Height[FRONT] + Height[RIGHT])))
    {
        getTiles()[Index].aColor = Lighter;
        return;
    }
    if ((Height[BACK] + Height[LEFT]) > (Height[FRONT] + Height[RIGHT]))
    {
        getTiles()[Index].aColor = Lighter;
        return;
    }
    if ((Height[BACK] + Height[LEFT]) < (Height[FRONT] + Height[RIGHT]))
    {
        getTiles()[Index].aColor = Darker;
        return;
    }
}