#pragma once

#include "Core.h"
#include "BitArray.h"
#include "Array.h"

#include "Templates/TypeCompatibleBytes.h"
#include "Templates/IsTriviallyDestructible.h"
#include "Templates/ChooseClass.h"



/** The result of a sparse array allocation. */
struct FSparseArrayAllocationInfo
{
	int32 Index;
	void* Pointer;
};

/** Allocated elements are overlapped with free element info in the element list. */
template<typename ElementType>
union TSparseArrayElementOrFreeListLink
{
	/** If the element is allocated, its value is stored here. */
	ElementType ElementData;

	struct
	{
		/** If the element isn't allocated, this is a link to the previous element in the array's free list. */
		int32 PrevFreeIndex;

		/** If the element isn't allocated, this is a link to the next element in the array's free list. */
		int32 NextFreeIndex;
	};
};

template<typename InElementType>
class TSparseArray
{
	using ElementType = InElementType;

	/**
	 * The element type stored is only indirectly related to the element type requested, to avoid instantiating TArray redundantly for
	 * compatible types.
	 */
	typedef TSparseArrayElementOrFreeListLink< TAlignedBytes<sizeof(ElementType), alignof(ElementType)> > FElementOrFreeListLink;

public:
	/** Destructor. */
	~TSparseArray()
	{
		// Destruct the elements in the array.
		Empty();
	}

	/**
	 * Allocates space for an element in the array.  The element is not initialized, and you must use the corresponding placement new operator
	 * to construct the element in the allocated memory.
	 */
	FSparseArrayAllocationInfo AddUninitialized()
	{
		int32 Index;
		if (NumFreeIndices)
		{
			// Remove and use the first index from the list of free elements.
			Index = FirstFreeIndex;
			FirstFreeIndex = GetData(FirstFreeIndex).NextFreeIndex;
			--NumFreeIndices;
			if (NumFreeIndices)
			{
				GetData(FirstFreeIndex).PrevFreeIndex = -1;
			}
		}
		else
		{
			// Add a new element.
			Index = Data.AddUninitialized(1);
			AllocationFlags.Add(false);
		}

		return AllocateIndex(Index);
	}

	/** Marks an index as allocated, and returns information about the allocation. */
	FSparseArrayAllocationInfo AllocateIndex(int32 Index)
	{
		ASSERT(Index >= 0);
		ASSERT(Index < GetMaxIndex());
		ASSERT(!AllocationFlags[Index]);

		// Flag the element as allocated.
		AllocationFlags[Index] = true;

		// Set the allocation info.
		FSparseArrayAllocationInfo Result;
		Result.Index = Index;
		Result.Pointer = &GetData(Result.Index).ElementData;

		return Result;
	}

	/** Adds an element to the array. */
	int32 Add(const ElementType& Element)
	{
		FSparseArrayAllocationInfo Allocation = AddUninitialized();
		new(Allocation) ElementType(Element);
		return Allocation.Index;
	}

	/** Adds an element to the array. */
	int32 Add(ElementType&& Element)
	{
		FSparseArrayAllocationInfo Allocation = AddUninitialized();
		new(Allocation) ElementType(MoveTemp(Element));
		return Allocation.Index;
	}

	/**
	 * Removes all elements from the array, potentially leaving space allocated for an expected number of elements about to be added.
	 * @param ExpectedNumElements - The expected number of elements about to be added.
	 */
	void Empty(int32 ExpectedNumElements = 0)
	{
		// Destruct the allocated elements.
		if (!TIsTriviallyDestructible<ElementType>::Value)
		{
			for (TIterator It(*this); It; ++It)
			{
				ElementType& Element = *It;
				Element.~ElementType();
			}
		}

		// Free the allocated elements.
		Data.Empty(ExpectedNumElements);
		FirstFreeIndex = -1;
		NumFreeIndices = 0;
		AllocationFlags.Empty(ExpectedNumElements);
	}

	/** Accessor for the element or free list data. */
	FElementOrFreeListLink& GetData(int32 Index)
	{
		return ((FElementOrFreeListLink*)Data.GetData())[Index];
	}

	/** Accessor for the element or free list data. */
	const FElementOrFreeListLink& GetData(int32 Index) const
	{
		return ((FElementOrFreeListLink*)Data.GetData())[Index];
	}

	// Accessors.
	ElementType& operator[](int32 Index)
	{
		ASSERT(Index >= 0 && Index < Data.Num() && Index < AllocationFlags.Num());
		//checkSlow(AllocationFlags[Index]); // Disabled to improve loading times -BZ
		return *(ElementType*)&GetData(Index).ElementData;
	}
	const ElementType& operator[](int32 Index) const
	{
		ASSERT(Index >= 0 && Index < Data.Num() && Index < AllocationFlags.Num());
		//checkSlow(AllocationFlags[Index]); // Disabled to improve loading times -BZ
		return *(ElementType*)&GetData(Index).ElementData;
	}

	int32 GetMaxIndex() const
	{
		return Data.Num();
	}

private:

	/** The base class of sparse array iterators. */
	template<bool bConst>
	class TBaseIterator
	{
	private:
		typedef typename TChooseClass<bConst, const TSparseArray, TSparseArray>::Result ArrayType;
		typedef typename TChooseClass<bConst, const ElementType, ElementType>::Result ItElementType;

	public:
		explicit TBaseIterator(ArrayType& InArray, const FConstSetBitIterator& InBitArrayIt)
			: Array(InArray)
			, BitArrayIt(InBitArrayIt)
		{
		}

		FORCEINLINE TBaseIterator& operator++()
		{
			// Iterate to the next set allocation flag.
			++BitArrayIt;
			return *this;
		}

		FORCEINLINE int32 GetIndex() const { return BitArrayIt.GetIndex(); }

		FORCEINLINE friend bool operator==(const TBaseIterator& Lhs, const TBaseIterator& Rhs) { return Lhs.BitArrayIt == Rhs.BitArrayIt && &Lhs.Array == &Rhs.Array; }
		FORCEINLINE friend bool operator!=(const TBaseIterator& Lhs, const TBaseIterator& Rhs) { return Lhs.BitArrayIt != Rhs.BitArrayIt || &Lhs.Array != &Rhs.Array; }

		/** conversion to "bool" returning true if the iterator is valid. */
		FORCEINLINE explicit operator bool() const
		{
			return !!BitArrayIt;
		}

		/** inverse of the "bool" operator */
		FORCEINLINE bool operator !() const
		{
			return !(bool)*this;
		}

		FORCEINLINE ItElementType& operator*() const { return Array[GetIndex()]; }
		FORCEINLINE ItElementType* operator->() const { return &Array[GetIndex()]; }
		FORCEINLINE const FRelativeBitReference& GetRelativeBitReference() const { return BitArrayIt; }

	protected:
		ArrayType&     Array;
		FConstSetBitIterator BitArrayIt;
	};

public:

	/** Iterates over all allocated elements in a sparse array. */
	class TIterator : public TBaseIterator<false>
	{
	public:
		TIterator(TSparseArray& InArray)
			: TBaseIterator<false>(InArray, FConstSetBitIterator(InArray.AllocationFlags))
		{
		}

		TIterator(TSparseArray& InArray, const FConstSetBitIterator& InBitArrayIt)
			: TBaseIterator<false>(InArray, InBitArrayIt)
		{
		}

		/** Safely removes the current element from the array. */
		void RemoveCurrent()
		{
			Array.RemoveAt(GetIndex());
		}
	};

	/** Iterates over all allocated elements in a const sparse array. */
	class TConstIterator : public TBaseIterator<true>
	{
	public:
		TConstIterator(const TSparseArray& InArray)
			: TBaseIterator<true>(InArray, FConstSetBitIterator(InArray.AllocationFlags))
		{
		}

		TConstIterator(const TSparseArray& InArray, const typename TBaseIterator<true>::BitArrayItType& InBitArrayIt)
			: TBaseIterator<true>(InArray, InBitArrayIt)
		{
		}
	};

	/**
	* DO NOT USE DIRECTLY
	* STL-like iterators to enable range-based for loop support.
	*/
	FORCEINLINE TIterator      begin() { return TIterator(*this); }
	FORCEINLINE TConstIterator begin() const { return TConstIterator(*this, FConstSetBitIterator(AllocationFlags)); }
	FORCEINLINE TIterator      end() { return TIterator(*this, FConstSetBitIterator(AllocationFlags, AllocationFlags.Num())); }
	FORCEINLINE TConstIterator end()   const { return TConstIterator(*this, FConstSetBitIterator(AllocationFlags, AllocationFlags.Num())); }

private:


	TArray<FElementOrFreeListLink> Data;
	FBitArray AllocationFlags;

	/** The index of an unallocated element in the array that currently contains the head of the linked list of free elements. */
	int32 FirstFreeIndex;

	/** The number of elements in the free list. */
	int32 NumFreeIndices;
};

/**
 * A placement new operator which constructs an element in a sparse array allocation.
 */
inline void* operator new(size_t Size, const FSparseArrayAllocationInfo& Allocation)
{
	return Allocation.Pointer;
}
