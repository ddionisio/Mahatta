#include "game_sequence.h"

namespace Sequence
{	
	/// @brief Constructs an Interval object
	/// @param owner Sequence used to build interval
	Interval::Interval (Sequence * owner) : mOwner(owner), mCount(0)
	{
	}

	/// @brief Destructs an Interval object
	Interval::~Interval (void)
	{
		mOwner->mIntervals.erase(this);
	}

	/// @brief Updates an interval after insertion
	/// @param where Position of insertion
	/// @param count Count of inserted values
	void Interval::AdjustForInsert (Uint where, Uint count)
	{
		if (0 == mCount) return;

		// If insertion occurred before the interval, move ahead by the insert count.
		if (where < mWhere) mWhere += count;

		// Otherwise, if inserting into the interval, augment it by the insert count.
		else if (where >= mWhere && where < mWhere + mCount) mCount += count;
	}

	/// @brief Updates an interval after removal
	/// @param where Position of removal
	/// @param count Count of removed values
	void Interval::AdjustForRemove (Uint where, Uint count)
	{
		if (0 == mCount) return;

		// If this overlaps the removal interval, reduce its count by the overlap amount.
		Uint begin = where, end = where + count;

		if (end > mWhere && begin < mWhere + mCount)
		{
			if (mWhere > begin)	begin = mWhere;

			if (mWhere + mCount < end) end = mWhere + mCount;

			mCount -= end - begin;
		}

		// If the interval follows the point of removal, it must be moved back. Reduce its
		// index by the lesser of the remove count and the distance from the point of
		// removal to the start of the interval.
		if (mWhere > where)
		{
			Uint adjust = mWhere - where;

			if (count < adjust) adjust = count;

			mWhere -= adjust;
		}
	}
}