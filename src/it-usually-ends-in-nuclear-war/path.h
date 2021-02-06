#ifndef IUEINW_PATH_H
#define IUEINW_PATH_H

namespace IUEINW
{
	class IUEINW_Path
	{
	public:
		IUEINW_Path() = default;
		fi::Slot_Map_ID UnitID;
		std::shared_ptr<std::deque<int>> Path = nullptr;
	};
}

#endif