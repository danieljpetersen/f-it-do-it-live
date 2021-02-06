#ifndef FI_RESOURCE_H
#define FI_RESOURCE_H

#include "slot_map.h"

// todo

namespace fi
{
	class Resource : public fi::Slot_Map_Item
	{
	public:
		int Value;
	};

	// ----

	class Resource_Modifier_Base
	{
	public:
		int Modifier; // todo -- everything should be fixedPoint for determinism
		fi::Slot_Map_ID Target;
		virtual void perform(Resource *Resource) = 0;
	};

	// ----

	class Resource_Modifier_Add : public Resource_Modifier_Base
	{
	public:
		void perform(Resource *Resource) override
		{
			Resource->Value += Modifier;
		}
	};

	// ----

	class Resource_Modifier_Multiply : public Resource_Modifier_Base
	{
	public:
		void perform(Resource *Resource) override
		{
			Resource->Value *= Modifier;
		}
	};

	// ----

	class Resource_Computation_Engine
	{
	public:
		fi::Slot_Map<fi::Resource> Data;
		std::vector<fi::Resource_Modifier_Base> Modifiers;
	};
}

#endif