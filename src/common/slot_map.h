#ifndef FI_SLOT_MAP_H
#define FI_SLOT_MAP_H

#include <vector>
#include <string>

////////////////////////////////////////////////////////////

namespace fi
{
    class Slot_Map_ID
    {
    public:
        Slot_Map_ID()
        {
            // Do not change this without also changing null()
            this->Version = -1;
            this->SlotMapIndex = -1;
        }

        static fi::Slot_Map_ID null()
        {
            // Do not change this without also changing the default constructor
            return { -1, -1 };
        }

        Slot_Map_ID(int SlotMapIndex, int Version)
        {
            this->SlotMapIndex = SlotMapIndex;
            this->Version = Version;
        }

        int SlotMapIndex=-1, Version=-1;

        fi::Slot_Map_ID getSlotMapID() const // convenience for inheriting classes
        {
            return { SlotMapIndex, Version };
        }

        bool operator!=(const Slot_Map_ID &other) const
        {
            return !((this->SlotMapIndex == other.SlotMapIndex) && (this->Version == other.Version));
        }
        bool operator==(const Slot_Map_ID &other) const
        {
            return ((this->SlotMapIndex == other.SlotMapIndex) && (this->Version == other.Version));
        }

        bool operator<(const Slot_Map_ID &other) const
        {
            return this->Version < other.Version;
        }

        bool operator>(const Slot_Map_ID &other) const
        {
            return this->Version > other.Version;
        }

        template <class Archive>
        void serialize( Archive & ar )
        {
            ar(this->Version);
            ar(this->SlotMapIndex);
        }

        template <class T>
        friend class Slot_Map;
    };

////////////////////////////////////////////////////////////

    class Slot_Map_Item_Interface
    {
    public:
        virtual Slot_Map_ID getSlotMapID() = 0;
        virtual const int getSlotMapIndex() = 0;
        virtual const  int getSlotMapVersion() = 0;
        virtual void setSlotMapID(Slot_Map_ID ID) = 0;
        virtual void setSlotMapIDVersion(int Version) = 0;
        virtual void setSlotMapIDSlotMapIndex(int SlotMapIndex) = 0;
    };

////////////////////////////////////////////////////////////

    class Slot_Map_Item : public Slot_Map_Item_Interface
    {
    public:
        Slot_Map_ID SlotMapID;

        Slot_Map_ID getSlotMapID() override
        {
            return SlotMapID;
        }

		const int getSlotMapIndex() override
        {
            return SlotMapID.SlotMapIndex;
        }

        virtual const int getSlotMapVersion() override
        {
            return SlotMapID.Version;
        }

        void setSlotMapID(Slot_Map_ID NewID) override
        {
            this->SlotMapID = NewID;
        }

        void setSlotMapIDVersion(int Version) override
        {
            SlotMapID.Version = Version;
        }

        void setSlotMapIDSlotMapIndex(int SlotMapIndex) override
        {
            SlotMapID.SlotMapIndex = SlotMapIndex;
        }
    };

////////////////////////////////////////////////////////////

    template <class T>
    class Slot_Map
    {
    protected:
        void _init()
        {
            _init(1);
        }

        void _init(unsigned long _InitialSizeToReserve)
        {
            clear();
            reserveSpace(_InitialSizeToReserve);
        }

        ////////////////////////////////////////////////////////////
        // user defined -- override me!
        virtual T* onObjectCreation(int ObjectIndex)
        {
            return &Objects[ObjectIndex];
        }

    public:
        Slot_Map()
        {
            _init();
        }

        std::vector<T> Objects;
        std::vector<int> SlotMap;// we need a unified SlotMap somehow, where each component's SlotMap is sized the same, but pointing to their actual index on their local vector
        std::vector<int> Freelist;
        unsigned long CurrentReserveCount=0;
        int VersionCount = 0;

        T   operator [](int i) const {return Objects[i];}
        T & operator [](int i)       {return Objects[i];}
		int size() { return (int)Objects.size(); }
		bool empty() { return Objects.empty(); }

        ////////////////////////////////////////////////////////////

        template <class Archive>
        void serialize( Archive & ar )
        {
            ar( Objects );
            ar( SlotMap );
            ar( Freelist );
            ar( CurrentReserveCount );
            ar( VersionCount );
        }

        ////////////////////////////////////////////////////////////

        T* createObject()
        {
            int SlotMapIndex;
            if (Freelist.size())
            {
                SlotMapIndex = Freelist.back();
                Freelist.pop_back();
            }
            else
            {
                reserveSpace(Objects.size() * 2);
                return createObject();
            }

            auto Count = (int)Objects.size();
            Objects.emplace_back(T());
            Objects[Count].setSlotMapIDVersion(VersionCount++);
            Objects[Count].setSlotMapIDSlotMapIndex(SlotMapIndex);  // pair with SlotMap
            SlotMap[SlotMapIndex] = Count;				 // pair with Components

            onObjectCreation(Count); // user defined initialization

            return &Objects[Count];
        }

        ////////////////////////////////////////////////////////////

        virtual void clear()
        {
            SlotMap.clear();
            Freelist.clear();
            Objects.clear();
            VersionCount = 0;

            reserveSpace(CurrentReserveCount);
        }

        ////////////////////////////////////////////////////////////

        void reserveSpace(unsigned long SIZE_TO_RESERVE)
        {
            SlotMap.reserve(SIZE_TO_RESERVE);
            Freelist.reserve(SIZE_TO_RESERVE);
            Objects.reserve(SIZE_TO_RESERVE);

            auto iSizeToReserve = (int)SIZE_TO_RESERVE;
            unsigned long iteratorCount = SIZE_TO_RESERVE - Objects.size();
            for (int _Iterator = 0; _Iterator < iteratorCount; _Iterator++)
            {
                Freelist.emplace_back(iSizeToReserve - _Iterator - 1);
                SlotMap.emplace_back(iSizeToReserve - _Iterator - 1);
            }

            if (this->CurrentReserveCount < SIZE_TO_RESERVE)
            {
                this->CurrentReserveCount = SIZE_TO_RESERVE;
            }
        }

        ////////////////////////////////////////////////////////////

        void remove(fi::Slot_Map_ID id)
        {
            if (isValid(id))
            {
                int Version = id.Version;
                int SlotMapIndex = id.SlotMapIndex;
                int EntityIndex = SlotMap[SlotMapIndex];
                if (Objects[EntityIndex].getSlotMapID().Version == Version)
                {
                    if (Objects.size() > 1)
                    {
                        std::swap(Objects[EntityIndex], Objects.back());      	 	// swap back
                        SlotMap[Objects[EntityIndex].getSlotMapIndex()] = EntityIndex;	 // update slot map for the now moved item
                    }

                    Objects.pop_back();										 	// remove back item
                    Freelist.emplace_back(SlotMapIndex);		 				// the slot on the slotmap which we just removed is free
                }
            }
        }

        ////////////////////////////////////////////////////////////

        void remove(int EntityIndex)
        {
            remove(Objects[EntityIndex].getSlotMapID());
        }

        ////////////////////////////////////////////////////////////

        bool isValid(fi::Slot_Map_ID id)
        {
            int SlotMapIndex = id.SlotMapIndex;
            int Version = id.Version;
            if (Version < 0)
            {
                return false;
            }

            if (SlotMapIndex >= SlotMap.size())
            {
                return false;
            }

            int Index = SlotMap[SlotMapIndex];

            if (Objects.size() > Index)
            {
                if (Objects[Index].getSlotMapID().Version == Version)
                {
                    return true;
                }
            }

            return false;
        }

        ////////////////////////////////////////////////////////////

        int getObjectIndex(fi::Slot_Map_ID id)
        {
            int SlotMapIndex = id.SlotMapIndex;
            if (SlotMapIndex < 0)
            {
                return -1;
            }

            if (SlotMapIndex >= SlotMap.size())
            {
                return -1;
            }

            return SlotMap[SlotMapIndex];
        }

        ////////////////////////////////////////////////////////////

        T *get(fi::Slot_Map_ID id)
        {
            int SlotMapIndex = id.SlotMapIndex;
            if (SlotMapIndex < 0)
            {
                return nullptr;
            }

            if (SlotMapIndex >= SlotMap.size())
            {
                return nullptr;
            }

            int Index = SlotMap[SlotMapIndex];

            // is object still valid?
            if (Objects.size() > Index)
            {
                if (Objects[Index].getSlotMapID().Version == id.Version)
                {
                    return &Objects[Index];
                }
            }

            return nullptr;
        }

        ////////////////////////////////////////////////////////////

    };

////////////////////////////////////////////////////////////

	class Slot_Map_Hash_Fn
	{
	public:
		size_t operator()(const fi::Slot_Map_ID& p) const
		{
			return (std::hash<int>()(p.Version)) ^
				   (std::hash<int>()(p.SlotMapIndex));
		}
	};
}

#endif