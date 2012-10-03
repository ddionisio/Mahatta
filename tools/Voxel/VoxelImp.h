#ifndef VOXEL_IMP_H
#define VOXEL_IMP_H

namespace Voxel
{
	/// @brief Triple indices
	enum TripleIndex {
		eTX,///< x-index
		eTY,///< y-index
		eTZ	///< z-index
	};

	/// @brief Cell coordinates
	struct Cell	{
		// Members
		int m[3];	///< Cell offsets

		// Lifetime
		Cell (float v[3], float dx, float dy, float dz, float delta);

		// Methods
		void Distances (TripleIndex index, float center[3], float dim[3], float & dL, float & dG);
	};

	/// @brief Base step information type
	struct StepInfo {
		// Lifetime
		virtual ~StepInfo (void) {};

		// Methods
		virtual bool operator == (StepInfo & si) = 0;

		virtual StepInfo * Copy (void) = 0;

		virtual int M1 (void) = 0;
		virtual int M2 (void) { return M1(); }
	};
}

#endif // VOXEL_IMP_H