#include "Sphere.h"

namespace Voxel
{
	/// @brief Column step information
	struct CStepInfo : StepInfo {
		// Members
		int mY;	///< Column cell

		/// @brief Compares column step information for equality
		/// @param si Column step information to compare
		/// @return Equality boolean
		bool operator == (StepInfo & si)
		{
			return mY == static_cast<CStepInfo&>(si).mY;
		}

		/// @brief Copies column step information
		/// @return Duplicate
		CStepInfo * Copy (void)
		{
			CStepInfo * csi = new CStepInfo;

			*csi = *this;

			return csi;
		}

		/// @brief Gets the column cell
		/// @return Cell
		int M1 (void)
		{
			return mY;
		}
	};

	/// @brief Row step information
	struct RStepInfo : StepInfo {
		// Members
		EntryList::iterator mFI;///< Forward entry list iterator
		EntryList::reverse_iterator mRI;///< Reverse entry list iterator
		bool mReverse;	///< If true, use reverse iterator

		/// @brief Compares row step information for equality
		/// @param si Row step information to compare
		/// @return Equality boolean
		bool operator == (StepInfo & si)
		{
			RStepInfo & csi = static_cast<RStepInfo&>(si);

			if (mReverse != csi.mReverse) return false;

			return mReverse ? (mRI == csi.mRI) : (mFI == csi.mFI);
		}

		/// @brief Copies row step information
		/// @return Duplicate
		RStepInfo * Copy (void)
		{
			RStepInfo * rsi = new RStepInfo;

			*rsi = *this;

			return rsi;
		}

		/// @brief Gets the span for this entry
		/// @param x1 [out] Initial span cell
		/// @param x2 [out] Final span cell
		void XSpan (int & x1, int & x2)
		{
			x1 = mFI->mX1;
			x2 = mFI->mX2;
		}

		/// @brief Gets the row cell
		/// @return Cell
		int M1 (void)
		{
			return mReverse ? mRI->mZ : mFI->mZ;
		}
	};

	/// @brief Span step information
	struct SStepInfo : StepInfo {
		// Members
		int mI;	///< Initial span cell
		int mF;	///< Final span cell
		bool mEnd;	///< If true, span iterator is at end

		/// @brief Compares span step information for equality
		/// @param si Span step information to compare
		/// @return Equality boolean
		bool operator == (StepInfo & si)
		{
			return mEnd == static_cast<SStepInfo&>(si).mEnd;
		}

		/// @brief Copies span step information
		/// @return Duplicate
		SStepInfo * Copy (void)
		{
			SStepInfo * ssi = new SStepInfo;

			*ssi = *this;

			return ssi;
		}

		/// @brief Gets the initial span cell
		/// @return Cell
		int M1 (void)
		{
			return mI;
		}

		/// @brief Gets the final span cell
		/// @return Cell
		int M2 (void)
		{
			return mF;
		}
	};

	const int eXY = 0x1;///< Swap x, y
	const int eXZ = 0x2;///< Swap x, z
	const int eYZ = 0x4;///< Swap y, z

	/// @brief Constructs a RenderData object
	/// @param center Sphere center, relative to space origin
	/// @param radius Sphere radius
	/// @param dx Extent of space cell in x-direction
	/// @param dy Extent of space cell in y-direction
	/// @param dz Extent of space cell in z-direction
	/// @param order Order used to obtain spans
	Sphere::Sphere (float center[3], float radius, float dx, float dy, float dz, Order order) : Data(dx, dy, dz),
																								mCenter(center, dx, dy, dz, 0.0f),
																								mMin(center, dx, dy, dz, -radius),
																								mMax(center, dx, dy, dz, +radius),
																								mOp(&EntryList::push_back),
																								mR2(radius * radius)
	{
		if (radius <= 0.0f) throw "Non-positive radius";

		// Convert the displacements from the given volume ordering to the standard form.
		int flags[] = { eXY | eYZ, eXZ, eYZ, 0, eXZ | eYZ, eXY };

		if (flags[order] & eXY) std::swap(mD[eTX], mD[eTY]), std::swap(center[eTX], center[eTY]);
		if (flags[order] & eXZ) std::swap(mD[eTX], mD[eTZ]), std::swap(center[eTX], center[eTZ]);
		if (flags[order] & eYZ) std::swap(mD[eTY], mD[eTZ]), std::swap(center[eTY], center[eTZ]);

		/// Install the column. Insert the known entry through the center.
		mColumn.resize(mMax.m[eTY] - mMin.m[eTY] + 1);

		AddEntry(mMin.m[eTX], mMax.m[eTX], mCenter.m[eTY], mCenter.m[eTZ]);

		// Get the distances from the center to each of the cell edges in its xz-plane.
		mCenter.Distances(eTX, center, mD, mXL, mXG);
		mCenter.Distances(eTZ, center, mD, mZL, mZG);

		// Get the distances from the center to the xz-planes that form the ceiling and
		// floor of the cells of the bottom and top points, respectively.
		mCenter.Distances(eTY, center, mD, mYL, mYG);

		Extend(eTY, mYL, mYG);

		// Do the z = z0 circle.
		ZCircle(mYL, mYG, 0.0f, mMin.m[eTY], mMax.m[eTY], mCenter.m[eTZ]);

		// Do the x = x0 and y = y0 circles.
		XYSemicircle(mZL, -1, mCenter.m[eTZ] - mMin.m[eTZ]);
		mOp = &EntryList::push_front;
		XYSemicircle(mZG, +1, mMax.m[eTZ] - mCenter.m[eTZ]);
	}

	/// @brief Gets an edge column iterator
	/// @param bEnd If true, get the end iterator
	/// @param bReverse If true, get reverse info
	/// @return New column step info
	StepInfo * Sphere::EdgeC (bool bEnd, bool bReverse)
	{
		CStepInfo * csi = new CStepInfo;

		if (bReverse) csi->mY = bEnd ? mMin.m[eTY] - 1 : mMax.m[eTY];

		else csi->mY = bEnd ? mMax.m[eTY] + 1 : mMin.m[eTY];

		return csi;
	}

	/// @brief Gets an edge row iterator
	/// @param csi Column step info
	/// @param bEnd If true, get the end iterator
	/// @param bReverse If true, get reverse info
	/// @return New row step info
	StepInfo * Sphere::EdgeR (StepInfo * csi, bool bEnd, bool bReverse)
	{
		EntryList & el = mColumn[static_cast<CStepInfo*>(csi)->mY - mMin.m[eTY]];

		RStepInfo * rsi = new RStepInfo;

		if (bReverse) rsi->mRI = bEnd ? el.rend() : el.rbegin();

		else rsi->mFI = bEnd ? el.end() : el.begin();
		
		rsi->mReverse = bReverse;

		return rsi;
	}

	/// @brief Gets an edge span iterator
	/// @param rsi Row step info
	/// @param bEnd If true, get the end iterator
	/// @param bReverse If true, get reverse info
	/// @return New span step info
	StepInfo * Sphere::EdgeS (StepInfo * rsi, bool bEnd, bool bReverse)
	{
		SStepInfo * ssi = new SStepInfo;

		if (bEnd) ssi->mEnd = true;

		else
		{
			ssi->mEnd = false;

			static_cast<RStepInfo*>(rsi)->XSpan(ssi->mI, ssi->mF);

			if (bReverse) std::swap(ssi->mI, ssi->mF);
		}

		return ssi;
	}

	/// @brief Adds an entry to the row
	/// @param x1 x-cell of span start
	/// @param x2 x-cell of span end
	/// @param y y-cell of row
	/// @param z z-cell of span
	void Sphere::AddEntry (int x1, int x2, int y, int z)
	{
		Entry entry;

		entry.mX1 = x1;
		entry.mX2 = x2;
		entry.mZ = z;

		(mColumn[y - mMin.m[eTY]].*mOp)(entry);
	}

	/// @brief Extends a distance to just short of the extrema cells
	/// @param index Coordinate index
	/// @param cL [out] Lesser coordinate to extend
	/// @param cG [out] Greater coordinate to extend
	void Sphere::Extend (TripleIndex index, float & cL, float & cG)
	{
		int dL = mCenter.m[index] - mMin.m[index] - 1;	if (dL > 0) cL += dL * mD[index];
		int dG = mMax.m[index] - mCenter.m[index] - 1;	if (dG > 0) cG += dG * mD[index];
	}

	/// @brief Steps along a column
	/// @param csi Column step info
	/// @param bReverse If true, step in reverse
	/// @param bDec If true, decrement
	void Sphere::StepC (StepInfo * csi, bool bReverse, bool bDec)
	{
		int dY = bDec ? -1 : +1;

		static_cast<CStepInfo*>(csi)->mY += bReverse ? -dY : +dY;
	}

	/// @brief Steps along a row
	/// @param rsi Row step info
	/// @param bReverse If true, step in reverse
	/// @param bDec If true, decrement
	void Sphere::StepR (StepInfo * rsi, bool bReverse, bool bDec)
	{
		RStepInfo * pRSI = static_cast<RStepInfo*>(rsi);

		if (bReverse) bDec ? --pRSI->mRI : ++pRSI->mRI;

		else bDec ? --pRSI->mFI : ++pRSI->mFI;
	}

	/// @brief Steps along a span
	/// @param ssi Span step info
	/// @param bReverse If true, step in reverse
	/// @param bDec If true, decrement
	void Sphere::StepS (StepInfo * ssi, bool bReverse, bool bDec)
	{
		static_cast<SStepInfo*>(ssi)->mEnd = true;
	}

	/// @brief Renders semicircles of x = x0, y = y0 and all their z-circles
	/// @param z z-value of start of x = x0 semicircle
	/// @param dZ z-cell increment
	/// @param count Count of z-cells to cover
	void Sphere::XYSemicircle (float z, int dZ, int count)
	{
		int cx1 = mMin.m[eTX], cx2 = mMax.m[eTX];
		int cy1 = mMin.m[eTY], cy2 = mMax.m[eTY];
		int cZ = mCenter.m[eTZ];

		float xL = mXL, xG = mXG;
		float yL = mYL, yG = mYG;

		Extend(eTX, xL, xG);

		for (int index = 0; index < count; ++index, z += mD[eTZ])
		{
			cZ += dZ;

			float resZ = mR2 - z * z;

			// Iterate inward along the yz-plane until r² - y² - z² >= 0. Until then,
			// the corner is poking out of the x0 semicircle. Render the semicircle.
			while (yL > mD[eTY] && yL * yL > resZ) yL -= mD[eTY], ++cy1;
			while (yG > mD[eTY] && yG * yG > resZ) yG -= mD[eTY], --cy2;

			ZCircle(yL, yG, z, cy1, cy2, cZ);

			// Iterate inward along the xz-plane until r² - x² - z² < 0. Until then,
			// the corner is poking out of the y0 semicircle. Install this entry.
			while (xL > mD[eTX] && xL * xL > resZ) xL -= mD[eTX], ++cx1;
			while (xG > mD[eTX] && xG * xG > resZ) xG -= mD[eTX], --cx2;

			AddEntry(cx1, cx2, mCenter.m[eTY], cZ);
		}
	}

	/// @brief Renders the circle at a given z
	/// @param fYL Lesser y-value
	/// @param fYG Greater y-value
	/// @param z z-value at which to render
	/// @param cyL Lesser y-cell
	/// @param cyG Greater y-cell
	/// @param cZ z-cell
	void Sphere::ZCircle (float yL, float yG, float z, int cyL, int cyG, int cZ)
	{
		float res = mR2 - z * z;

		ZSemicircle(yL, res, cyL, +1, cZ);
		ZSemicircle(yG, res, cyG, -1, cZ);
	}

	/// @brief Renders a semicircle for a given z
	/// @param y y-value of start of semicircle
	/// @param res Residue term, r² - z²
	/// @param cY y-cell of start of semicircle
	/// @param dY y-cell increment
	/// @param cZ z-cell
	void Sphere::ZSemicircle (float y, float res, int cY, int dY, int cZ)
	{
		int cx1 = mCenter.m[eTX], cx2 = mCenter.m[eTX];

		for (float xL = mXL, xG = mXG; cY != mCenter.m[eTY]; y -= mD[eTY], cY += dY)
		{
			float resY = res - y * y;

			// Iterate along the z semicircle until r² - x² - y² - z² < 0. Then, the
			// corner is poking out of the z semicircle. Install this entry.
			while (xL * xL <= resY) xL += mD[eTX], --cx1;
			while (xG * xG <= resY) xG += mD[eTX], ++cx2;

			AddEntry(cx1, cx2, cY, cZ);
		}
	}
}