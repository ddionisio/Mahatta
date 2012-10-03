#include "game_sequence.h"

namespace Sequence
{
	/// @brief Invalidates the spot
	void Spot::Clear (void)
	{
		mStatus.reset(eValid);
	}

	/// @brief Removes the spot from the range
	void Spot::Remove (void)
	{
		delete this;
	}

	/// @brief Assigns the position
	/// @param where Position to assign
	void Spot::Set (Uint where)
	{
		if (!mOwner->IsItemValid(where, mStatus.test(eAddSpot))) return;

		mWhere = where;

		mStatus.set(eValid);
	}

	/// @brief Gets the position of a spot
	/// @param where [out] On success, spot position
	/// @return If true, success
	bool Spot::Get (Uint & where)
	{
		if (!Valid()) return false;

		where = mWhere;

		return true;
	}
}