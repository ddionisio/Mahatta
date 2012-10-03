#include "Voxel.h"
#include "VoxelImp.h"
#include <cmath>

namespace Voxel
{
	/// @brief Constructs a cell
	/// @param v Point in space
	/// @param dx Extent of space cell in x-direction
	/// @param dy Extent of space cell in y-direction
	/// @param dz Extent of space cell in z-direction
	/// @param delta Displacement on (x, y, z) line from point
	Cell::Cell (float v[3], float dx, float dy, float dz, float delta)
	{
		m[eTX] = int(ceilf((v[0] + delta) / dx - 0.5f));
		m[eTY] = int(ceilf((v[1] + delta) / dy - 0.5f));
		m[eTZ] = int(ceilf((v[2] + delta) / dz - 0.5f));
	}

	/// @brief Get the distances from the center to a pair of cell walls
	/// @param index Coordinate index
	/// @param center Sphere center
	/// @param dim Dimension array
	/// @param dL [out] Distance to wall with lesser coordinate
	/// @param dG [out] Distance to wall with greater coordinate
	void Cell::Distances (TripleIndex index, float center[3], float dim[3], float & dL, float & dG)
	{
		dG = (m[index] + 0.5f) * dim[index] - center[index], dL = dim[index] - dG;
	}

	/// @brief Constructs a Data object
	/// @param dx Extent of space cell in x-direction
	/// @param dy Extent of space cell in y-direction
	/// @param dz Extent of space cell in z-direction
	Data::Data (float dx, float dy, float dz)
	{
		if (dx <= 0.0f) throw "Non-positive dx";
		if (dy <= 0.0f) throw "Non-positive dy";
		if (dz <= 0.0f) throw "Non-positive dz";

		mD[eTX] = dx;
		mD[eTY] = dy;
		mD[eTZ] = dz;
	}

	/// @brief Gets the voxel data's begin forward iterator
	/// @return Forward iterator
	ColumnIterF Data::begin (void)
	{
		return ColumnIterF(EdgeC(false, false), this);
	}

	/// @brief Gets the voxel data's end forward iterator
	/// @return Forward iterator
	ColumnIterF Data::end (void)
	{
		return ColumnIterF(EdgeC(true, false), this);
	}

	/// @brief Gets the voxel data's begin reverse iterator
	/// @return Reverse iterator
	ColumnIterR Data::rbegin (void)
	{
		return ColumnIterR(EdgeC(false, true), this);
	}

	/// @brief Gets the voxel data's end reverse iterator
	/// @return Reverse iterator
	ColumnIterR Data::rend (void)
	{
		return ColumnIterR(EdgeC(true, true), this);
	}

	/// @brief Destructs a Iter object
	Iter::~Iter (void)
	{
		delete mSI;
	}

	/// @brief Compares two iterators for equality
	/// @param v Iterator to compare
	/// @return Equality boolean
	bool Iter::operator == (Iter const & v)
	{
		return *mSI == *v.mSI;
	}

	/// @brief Compares two iterators for inequality
	/// @param v Iterator to compare
	/// @return Inequality boolean
	bool Iter::operator != (Iter const & v)
	{
		return !(*mSI == *v.mSI);
	}
}