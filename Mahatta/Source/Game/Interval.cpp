#include "game_sequence.h"

namespace Sequence
{
	/// @brief Clears the selection
	void Interval::Clear (void)
	{
		mCount = 0;
	}

	/// @brief Removes the interval from the range
	void Interval::Remove (void)
	{
		delete this;
	}

	/// @brief Gets the starting position and select count
	/// @param where [out] If non-empty, the starting position
	/// @return Count of selected items
	Uint Interval::Get (Uint & where)
	{
		if (0 == mCount) return 0;

		where = mWhere;

		return mCount;
	}

	/// @brief Makes a selection
	/// @param where Index from which to begin selection
	/// @param count Count of items to select
	/// @return Count of items selected
	Uint Interval::Set (Uint where, Uint count)
	{
		if (!mOwner->IsItemValid(where, true)) return 0;

		mWhere = where;
		mCount = mOwner->Trim(where, count);

		return mCount;
	}
}