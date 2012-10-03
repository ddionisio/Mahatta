#include "Voxel.h"
#include "VoxelImp.h"

namespace Voxel
{
	/// @brief Dereferences the row iterator
	/// @return Row cell
	int RowIter::operator * (void)
	{
		 return mSI->M1();
	}

	/// @brief Gets the row's begin forward iterator
	/// @return Forward iterator
	SpanIterF RowIter::begin (void)
	{
		return SpanIterF(mVD->EdgeS(mSI, false, false), mVD);
	}

	/// @brief Gets the row's end forward iterator
	/// @return Forward iterator
	SpanIterF RowIter::end (void)
	{
		return SpanIterF(mVD->EdgeS(mSI, true, false), mVD);
	}

	/// @brief Gets the row's begin reverse iterator
	/// @return Reverse iterator
	SpanIterR RowIter::rbegin (void)
	{
		return SpanIterR(mVD->EdgeS(mSI, false, true), mVD);
	}

	/// @brief Gets the row's end reverse iterator
	/// @return Reverse iterator
	SpanIterR RowIter::rend (void)
	{
		return SpanIterR(mVD->EdgeS(mSI, true, true), mVD);
	}

	/// @brief Copy constructs a RowIterF object
	/// @param vo RowIterF object from which to copy
	RowIterF::RowIterF (RowIterF const & vo) : RowIter(vo.mSI->Copy(), vo.mVD)
	{
	}

	/// @brief Increments the forward row iterator
	void RowIterF::operator ++ (void)
	{
		mVD->StepR(mSI, false, false);
	}

	/// @brief Decrements the forward row iterator
	void RowIterF::operator -- (void)
	{
		mVD->StepR(mSI, false, true);
	}

	/// @brief Copy constructs a RowIterR object
	/// @param vo RowIterR object from which to copy
	RowIterR::RowIterR (RowIterR const & vo) : RowIter(vo.mSI->Copy(), vo.mVD)
	{
	}

	/// @brief Increments the reverse row iterator
	void RowIterR::operator ++ (void)
	{
		mVD->StepR(mSI, true, false);
	}

	/// @brief Decrements the reverse row iterator
	void RowIterR::operator -- (void)
	{
		mVD->StepR(mSI, true, true);
	}
}