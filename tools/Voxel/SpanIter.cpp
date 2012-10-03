#include "Voxel.h"
#include "VoxelImp.h"

namespace Voxel
{
	/// @brief Gets the initial element of the span
	/// @return Initial cell
	int SpanIter::I (void)
	{
		return mSI->M1();
	}

	/// @brief Gets the final element of the span
	/// @return Final cell
	int SpanIter::F (void)
	{
		return mSI->M2();
	}

	/// @brief Copy constructs a SpanIterF object
	/// @param vo SpanIterF object from which to copy
	SpanIterF::SpanIterF (SpanIterF const & vo) : SpanIter(vo.mSI->Copy(), vo.mVD)
	{
	}

	/// @brief Increments the forward span iterator
	void SpanIterF::operator ++ (void)
	{
		mVD->StepS(mSI, false, false);
	}

	/// @brief Decrements the forward span iterator
	void SpanIterF::operator -- (void)
	{
		mVD->StepS(mSI, false, true);
	}

	/// @brief Copy constructs a SpanIterR object
	/// @param vo SpanIterR object from which to copy
	SpanIterR::SpanIterR (SpanIterR const & vo) : SpanIter(vo.mSI->Copy(), vo.mVD)
	{
	}

	/// @brief Increments the reverse span iterator
	void SpanIterR::operator ++ (void)
	{
		mVD->StepS(mSI, true, false);
	}

	/// @brief Decrements the reverse span iterator
	void SpanIterR::operator -- (void)
	{
		mVD->StepS(mSI, true, true);
	}
}