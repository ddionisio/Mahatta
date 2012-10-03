#include "game_sequence.h"

namespace Sequence
{
	/// @brief Inserts new items
	/// @param where Index of insertion
	/// @param count Count of items to add
	void Sequence::InsertItems (Uint where, Uint count)
	{
		if (!IsItemValid(where, true)) return;

		// Adjust each used spot and interval for insertion. Document the insertion.
		for (IterS_S sIter = mSpots.begin(); sIter != mSpots.end(); ++sIter)
		{
			(*sIter)->AdjustForInsert(where, count);
		}

		for (IterI_S iIter = mIntervals.begin(); iIter != mIntervals.end(); ++iIter)
		{
			(*iIter)->AdjustForInsert(where, count);
		}

		mItemCount += count;
	}

	/// @brief Creates a custom interval
	/// @return 0 on failure; spot on success
	Interval * Sequence::CreateInterval (void)
	{
		Interval * interval = 0;

		try {
			interval = new Interval(this);

			mIntervals.insert(interval);
		} catch (std::bad_alloc &) {}

		return interval;
	}

	/// @brief Adds a custom spot
	/// @param bAddSpot If true, the spot may occupy the end of the range
	/// @param bMigrate If true, the spot will migrate if deleted
	/// @return 0 on failure; spot on success
	Spot * Sequence::CreateSpot (bool bAddSpot, bool bMigrate)
	{
		Spot * spot = 0;

		try {
			spot = new Spot(this, bAddSpot, bMigrate);

			mSpots.insert(spot);
		} catch (std::bad_alloc &) {}

		return spot;
	}

	/// @brief Indicates whether an item is valid
	/// @param item Index of item
	/// @param bAddItem If true, the end of the range is valid
	/// @return If true, the item is valid
	bool Sequence::IsItemValid (Uint where, bool bAddItem)
	{
		Uint extent = mItemCount;

		if (bAddItem) ++extent;

		return where < extent;
	}

	/// @brief Gets the item count
	/// @return Item count
	Uint Sequence::GetItemCount (void)
	{
		return mItemCount;
	}

	/// @brief Removes a series of items
	/// @param where Index of removal
	/// @param count Count of items to remove
	/// @return Count of items removed
	Uint Sequence::RemoveItems (Uint where, Uint count)
	{
		if (!IsItemValid(where, false)) return 0;

		// Trim the count to the range.
		count = Trim(where, count);

		// Adjust each used spot and interval for deletion. Document the deletion.
		for (IterS_S sIter = mSpots.begin(); sIter != mSpots.end(); ++sIter)
		{
			(*sIter)->AdjustForRemove(where, count);
		}

		for (IterI_S iIter = mIntervals.begin(); iIter != mIntervals.end(); ++iIter)
		{
			(*iIter)->AdjustForRemove(where, count);
		}

		mItemCount -= count;

		return count;
	}

	/// @brief Trims the range [start, start + count) to [start, size)
	/// @param start Index of the start of the range
	/// @param count Count of items in the range
	/// @return Modified item count
	Uint Sequence::Trim (Uint start, Uint count)
	{
		if (start >= mItemCount) return 0;

		return start + count > mItemCount ? mItemCount - start : count;
	}
}