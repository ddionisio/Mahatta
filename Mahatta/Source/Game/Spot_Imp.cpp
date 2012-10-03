#include "game_sequence.h"

namespace Sequence
{
	/// @brief Constructs a Spot object
	/// @param owner Sequence used to build spot
	/// @param bAddSpot Indicates whether spot can occupy end of range
	/// @param bMigrate Indicates whether spot can migrate if removed
	Spot::Spot (Sequence * owner, bool bAddSpot, bool bMigrate) : mOwner(owner), mWhere(0)
	{
		mStatus.set(eAddSpot, bAddSpot);
		mStatus.set(eMigrate, bMigrate);

		if (bMigrate) mStatus.set(eValid, Valid());
	}

	/// @brief Destructs a Spot object
	Spot::~Spot (void)
	{
		mOwner->mSpots.erase(this);
	}

	/// @brief Updates a spot after insertion
	/// @param where Position of insertion
	/// @param count Count of inserted values
	void Spot::AdjustForInsert (Uint where, Uint count)
	{
		if (!Valid()) return;
		
		// Move the spot up if it is not before the insert position.
		if (mWhere >= where) mWhere += count;

		// If the sequence is empty, the spot will be after the range. Move it back if this
		// is not allowed.
		if (0 == mOwner->mItemCount && !mStatus.test(eAddSpot)) --mWhere;
	}

	/// @brief Updates a spot after removal
	/// @param where Position of removal
	/// @param count Count of removed values
	void Spot::AdjustForRemove (Uint where, Uint count)
	{
		if (!Valid()) return;

		// If a migratory spot follows the point of removal, it must be moved back. Reduce
		// its index by the lesser of the remove count and the distance from the point of
		// removal to the index.
		if (mStatus.test(eMigrate))
		{
			if (mWhere > where)
			{
				Uint adjust = mWhere - where;

				if (count < adjust) adjust = count;

				mWhere -= adjust;
			}

			// If a block was removed at the end of the items, and it included the spot,
			// the spot will now be after the items. Move it back if this is not allowed.
			if (mWhere + count == mOwner->mItemCount)
			{
				if (mStatus.test(eAddSpot) || count == mOwner->mItemCount) return;
				
				--mWhere;
			}
		}

		// If a non-migratory spot follows the interval being removed, move it back by the
		// remove count; clear the spot if it is within the interval.
		else
		{
			if (mWhere >= where + count) mWhere -= count;

			else if (mWhere >= where) Clear();
		}
	}

	/// @brief Indicates spot validity
	/// @return If true, the spot is valid
	bool Spot::Valid (void)
	{
		if (!mStatus.test(eMigrate) && !mStatus.test(eValid)) return false;
		if (!mOwner->IsItemValid(mWhere, mStatus.test(eAddSpot))) return false;

		return true;
	}
}