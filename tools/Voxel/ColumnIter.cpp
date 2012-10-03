#include "Voxel.h"
#include "VoxelImp.h"

namespace Voxel
{
	/// @brief Derefernces the column iterator
	/// @return Column cell
	int ColumnIter::operator * (void)
	{
		return mSI->M1();
	}

	/// @brief Gets the column's begin forward iterator
	/// @return Forward iterator
	RowIterF ColumnIter::begin (void)
	{
		return RowIterF(mVD->EdgeR(mSI, false, false), mVD);
	}

	/// @brief Gets the column's end forward iterator
	/// @return Forward iterator
	RowIterF ColumnIter::end (void)
	{
		return RowIterF(mVD->EdgeR(mSI, true, false), mVD);
	}

	/// @brief Gets the column's begin reverse iterator
	/// @return Reverse iterator
	RowIterR ColumnIter::rbegin (void)
	{
		return RowIterR(mVD->EdgeR(mSI, false, true), mVD);
	}

	/// @brief Gets the column's end reverse iterator
	/// @return Reverse iterator
	RowIterR ColumnIter::rend (void)
	{
		return RowIterR(mVD->EdgeR(mSI, true, true), mVD);
	}

	/// @brief Copy constructs a ColumnIterF object
	/// @param vo ColumnIterF object from which to copy
	ColumnIterF::ColumnIterF (ColumnIterF const & vo) : ColumnIter(vo.mSI->Copy(), vo.mVD)
	{
	}

	/// @brief Increments the forward column iterator
	void ColumnIterF::operator ++ (void)
	{
		mVD->StepC(mSI, false, false);
	}

	/// @brief Decrements the forward column iterator
	void ColumnIterF::operator -- (void)
	{
		mVD->StepC(mSI, false, true);
	}

	/// @brief Copy constructs a ColumnIterR object
	/// @param vo ColumnIterR object from which to copy
	ColumnIterR::ColumnIterR (ColumnIterR const & vo) : ColumnIter(vo.mSI->Copy(), vo.mVD)
	{
	}

	/// @brief Increments the reverse column iterator
	void ColumnIterR::operator ++ (void)
	{
		mVD->StepC(mSI, true, false);
	}

	/// @brief Decrements the reverse column iterator
	void ColumnIterR::operator -- (void)
	{
		mVD->StepC(mSI, true, true);
	}
}