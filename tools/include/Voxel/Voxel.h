#ifndef VOXEL_H
#define VOXEL_H

#include <list>
#include <vector>

namespace Voxel
{
	/// @brief Ordering in which to iterate volume
	enum Order {
		eXYZ,	///< x, y, z ordering
		eXZY,	///< x, z, y ordering
		eYXZ,	///< y, x, z ordering
		eYZX,	///< y, z, x ordering
		eZXY,	///< z, x, y ordering
		eZYX	///< z, y, x ordering
	};

	// Forward references
	struct StepInfo;
	class ColumnIter;
	class ColumnIterF;
	class ColumnIterR;
	class RowIter;
	class RowIterF;
	class RowIterR;
	class SpanIter;
	class SpanIterF;
	class SpanIterR;

	/// @brief Data base class
	class Data {
	protected:
		// Members
		float mD[3];///< Dimensions, in standard form

		// Lifetime
		Data (float dx, float dy, float dz);
		virtual ~Data (void) {}

		// Methods
		virtual StepInfo * EdgeC (bool bEnd, bool bReverse) = 0;
		virtual StepInfo * EdgeR (StepInfo * csi, bool bEnd, bool bReverse) = 0;
		virtual StepInfo * EdgeS (StepInfo * rsi, bool bEnd, bool bReverse) = 0;
		virtual void StepC (StepInfo * csi, bool bReverse, bool bDec) = 0;
		virtual void StepR (StepInfo * rsi, bool bReverse, bool bDec) = 0;
		virtual void StepS (StepInfo * ssi, bool bReverse, bool bDec) = 0;

		// Friendship
		friend class ColumnIter;
		friend class ColumnIterF;
		friend class ColumnIterR;
		friend class RowIter;
		friend class RowIterF;
		friend class RowIterR;
		friend class SpanIter;
		friend class SpanIterF;
		friend class SpanIterR;
	public:
		// Methods
		ColumnIterF begin (void);
		ColumnIterR rbegin (void);
		ColumnIterF end (void);
		ColumnIterR rend (void);
	};

	/// @brief Base iterator type
	class Iter {
	protected:
		// Members
		StepInfo * mSI;	///< Step info
		Data * mVD;	///< Voxel data

		// Lifetime
		Iter (StepInfo * si, Data * vd) : mSI(si), mVD(vd) {}
		virtual ~Iter (void);
	public:
		// Methods
		bool operator == (Iter const & v);
		bool operator != (Iter const & v);
	};

	/// @brief Base column iterator
	class ColumnIter : public Iter {
	protected:
		// Lifetime
		ColumnIter (StepInfo * si, Data * vd) : Iter(si, vd) {}
	public:
		// Methods
		int operator * (void);

		RowIterF begin (void);
		RowIterF end (void);
		RowIterR rbegin (void);
		RowIterR rend (void);

		virtual void operator ++ (void) = 0;
		virtual void operator -- (void) = 0;
	};

	/// @brief Forward column iterator
	class ColumnIterF : public ColumnIter {
	private:
		// Lifetime
		ColumnIterF (StepInfo * si, Data * vd) : ColumnIter(si, vd) {}

		// Friendship
		friend class Data;
	public:
		// Lifetime
		ColumnIterF (ColumnIterF const & vo);

		// Methods
		void operator ++ (void);
		void operator -- (void);
	};

	/// @brief Reverse column iterator
	class ColumnIterR : public ColumnIter {
	private:
		// Lifetime
		ColumnIterR (StepInfo * si, Data * vd) : ColumnIter(si, vd) {}

		// Friendship
		friend class Data;
	public:
		// Lifetime
		ColumnIterR (ColumnIterR const & vo);

		// Methods
		void operator ++ (void);
		void operator -- (void);
	};

	/// @brief Base row iterator
	class RowIter : public Iter {
	protected:
		// Lifetime
		RowIter (StepInfo * si, Data * vd) : Iter(si, vd) {}
	public:
		// Methods
		int operator * (void);

		SpanIterF begin (void);
		SpanIterF end (void);
		SpanIterR rbegin (void);
		SpanIterR rend (void);

		virtual void operator ++ (void) = 0;
		virtual void operator -- (void) = 0;
	};

	/// @brief Forward row iterator
	class RowIterF : public RowIter {
	private:
		// Lifetime
		RowIterF (StepInfo * si, Data * vd) : RowIter(si, vd) {}

		// Friendship
		friend class ColumnIter;
	public:
		// Lifetime
		RowIterF (RowIterF const & vo);

		// Methods
		void operator ++ (void);
		void operator -- (void);
	};

	/// @brief Reverse row iterator
	class RowIterR : public RowIter {
	private:
		// Lifetime
		RowIterR (StepInfo * si, Data * vd) : RowIter(si, vd) {}

		// Friendship
		friend class ColumnIter;
	public:
		// Lifetime
		RowIterR (RowIterR const & vo);

		// Methods
		void operator ++ (void);
		void operator -- (void);
	};

	/// @brief Base span iterator
	class SpanIter : public Iter {
	protected:
		// Lifetime
		SpanIter (StepInfo * si, Data * vd) : Iter(si, vd) {}
	public:
		// Methods
		int I (void);
		int F (void);

		virtual void operator ++ (void) = 0;
		virtual void operator -- (void) = 0;
	};

	/// @brief Forward span iterator
	class SpanIterF : public SpanIter {
	private:
		// Lifetime
		SpanIterF (StepInfo * si, Data * vd) : SpanIter(si, vd) {}

		// Friendship
		friend class RowIter;
	public:
		// Lifetime
		SpanIterF (SpanIterF const & vo);

		// Methods
		void operator ++ (void);
		void operator -- (void);
	};

	/// @brief Reverse span iterator
	class SpanIterR : public SpanIter {
	private:
		// Lifetime
		SpanIterR (StepInfo * si, Data * vd) : SpanIter(si, vd) {}

		// Friendship
		friend class RowIter;
	public:
		// Lifetime
		SpanIterR (SpanIterR const & vo);

		// Methods
		void operator ++ (void);
		void operator -- (void);
	};
}

#endif // VOXEL_H