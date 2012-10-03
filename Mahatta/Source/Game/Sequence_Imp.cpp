#include "game_sequence.h"

namespace Sequence
{
	/// @brief Constructs a Sequence object
	Sequence::Sequence (void) : mItemCount(0)
	{
	}

	/// @brief Destructs a Sequence object
	Sequence::~Sequence (void)
	{
		while (!mSpots.empty()) (*mSpots.begin())->Remove();
		while (!mIntervals.empty()) (*mIntervals.begin())->Remove();		
	}
}