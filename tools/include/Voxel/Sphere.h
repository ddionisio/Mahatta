#ifndef VOXEL_SPHERE_H
#define VOXEL_SPHERE_H

#include "Voxel.h"
#include "VoxelImp.h"
#include <list>

namespace Voxel
{
	/// @brief Sphere entry
	struct Entry {
		int mX1;///< x-cell of span start
		int mX2;///< x-cell of span end
		int mZ;	///< z-cell of span
	};

	typedef std::list<Entry> EntryList;

	/// @brief Sphere data
	struct Sphere : public Data {
		// Members
		std::vector<EntryList> mColumn;	///< Column of span lists
		Cell mCenter;	///< Center cell values
		Cell mMin;	///< Minimum cell values
		Cell mMax;	///< Maximum cell values
		void (EntryList::* mOp)(Entry const &);	///< Operation used to add span to row
		float mXL;	///< Distance of center from lesser x-plane
		float mXG;	///< Distance of center from greater x-plane
		float mYL;	///< Distance of center from y-plane above bottom point
		float mYG;	///< Distance of center from y-plane below top point
		float mZL;	///< Distance of center from lesser z-plane
		float mZG;	///< Distance of center from greater z-plane
		float mR2;	///< Cached squared radius

		// Lifetime
		Sphere (float center[3], float radius, float dx, float dy, float dz, Order order);

		// Methods
		StepInfo * EdgeC (bool bEnd, bool bReverse);
		StepInfo * EdgeR (StepInfo * csi, bool bEnd, bool bReverse);
		StepInfo * EdgeS (StepInfo * rsi, bool bEnd, bool bReverse);

		void AddEntry (int x1, int x2, int y, int z);
		void Extend (TripleIndex index, float & cL, float & cG);
		void StepC (StepInfo * csi, bool bReverse, bool bDec);
		void StepR (StepInfo * rsi, bool bReverse, bool bDec);
		void StepS (StepInfo * ssi, bool bReverse, bool bDec);
		void XYSemicircle (float z, int dZ, int count);
		void ZCircle (float yL, float yG, float z, int cyL, int cyG, int cZ);
		void ZSemicircle (float y, float res, int cY, int dY, int cZ);
	};
}

#endif // VOXEL_SPHERE_H