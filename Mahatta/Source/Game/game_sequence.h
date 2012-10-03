#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "game_share.h"
#include <bitset>
#include <set>

namespace Sequence
{
	/// @brief Representation of a custom interval
	class Interval {
	private:
	// Members
		class Sequence * mOwner;///< Widget to which interval belongs
		Uint mCount;///< Count of items in interval
		Uint mWhere;///< Index of start of interval
	// Methods
		Interval (Sequence * owner);
		~Interval (void);

		void AdjustForInsert (Uint where, Uint count);
		void AdjustForRemove (Uint where, Uint count);
	// Friendship
		friend class Sequence;
	public:
	// Interface
		void Clear (void);
		void Remove (void);

		Uint Get (Uint & where);
		Uint Set (Uint where, Uint count);
	};

	/// @brief Representation of a custom spot
	class Spot {
	private:
	// Members
		class Sequence * mOwner;///< Sequence to which spot belongs
		std::bitset<sizeof(Uint) * 8> mStatus;	///< Current status of spot
		Uint mWhere;///< Index of spot
	// Enumerations
		enum {
			eAddSpot,	///< Spot can occupy end of range
			eMigrate,	///< Spot can migrate if removed
			eValid	///< Spot is valid
		};
	// Methods
		Spot (Sequence * owner, bool bAddSpot, bool bMigrate);
		~Spot (void);

		void AdjustForInsert (Uint where, Uint count);
		void AdjustForRemove (Uint where, Uint count);

		bool Valid (void);
	// Friendship
		friend class Sequence;
	public:
	// Interface
		void Clear (void);
		void Remove (void);
		void Set (Uint where);

		bool Get (Uint & where);
	};

	/// @brief Representation of a sequence
	class Sequence {
	private:
	// Types
		typedef std::set<Interval*>::iterator IterI_S;	///< Interval set iterator
		typedef std::set<Spot*>::iterator IterS_S;	///< Spot set iterator
	// Members
		std::set<Interval*> mIntervals;	///< Custom interval storage
		std::set<Spot*> mSpots;	///< Custom spot storage
		Uint mItemCount;///< Count of items
	// Friendship
		friend class Interval;
		friend class Spot;
	public:
	// Lifetime
		Sequence (void);
		~Sequence (void);
	// Interface
		void InsertItems (Uint where, Uint count);

		Interval * CreateInterval (void);

		Spot * CreateSpot (bool bAddSpot, bool bMigrate);

		bool IsItemValid (Uint where, bool bAddItem);

		Uint GetItemCount (void);
		Uint RemoveItems (Uint where, Uint count);
		Uint Trim (Uint where, Uint count);
	};
}

#endif // SEQUENCE_H