#pragma once

#include "Core/CoreTypes.h"
#include "BitArray.h"
#include "Array.h"

#include "Core/Templates/TypeCompatibleBytes.h"
#include "Core/Templates/IsTriviallyDestructible.h"
#include "Core/Templates/ChooseClass.h"



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

/**
 * A dynamically sized array where element indices aren't necessarily contiguous.  Memory is allocated for all
 * elements in the array's index range, so it doesn't save memory; but it does allow O(1) element removal that
 * doesn't invalidate the indices of subsequent elements.  It uses TArray to store the elements, and a TBitArray
 * to store whether each element index is allocated (for fast iteration over allocated elements).
 *
 **/

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

	/** Default constructor. */
	TSparseArray()
		: FirstFreeIndex(-1)
		, NumFreeIndices(0)
	{}

	/** Move constructor. */
	TSparseArray(TSparseArray&& InCopy)
	{
		Move(*this, InCopy);
	}

	/** Copy constructor. */
	TSparseArray(const TSparseArray& InCopy)
		: FirstFreeIndex(-1)
		, NumFreeIndices(0)
	{
		*this = InCopy;
	}

	/** Move assignment operator. */
	TSparseArray& operator=(TSparseArray&& InCopy)
	{
		if (this != &InCopy)
		{
			Move(*this, InCopy);
		}
		return *this;
	}

	/** Copy assignment operator. */
	TSparseArray& operator=(const TSparseArray& InCopy)
	{
		if (this != &InCopy)
		{
			int32 SrcMax = InCopy.GetMaxIndex();

			// Reallocate the array.
			Empty(SrcMax);
			Data.AddUninitialized(SrcMax);

			// Copy the other array's element allocation state.
			FirstFreeIndex = InCopy.FirstFreeIndex;
			NumFreeIndices = InCopy.NumFreeIndices;
			AllocationFlags = InCopy.AllocationFlags;

			// Determine whether we need per element construction or bulk copy is fine
			if (!TIsTriviallyCopyConstructible<ElementType>::Value)
			{
				FElementOrFreeListLink* DestData = (FElementOrFreeListLink*)Data.GetData();
				const FElementOrFreeListLink* SrcData = (FElementOrFreeListLink*)InCopy.Data.GetData();

				// Use the inplace new to copy the element to an array element
				for (int32 Index = 0; Index < SrcMax; ++Index)
				{
					FElementOrFreeListLink& DestElement = DestData[Index];
					const FElementOrFreeListLink& SrcElement = SrcData[Index];
					if (InCopy.IsAllocated(Index))
					{
						::new((uint8*)&DestElement.ElementData) ElementType(*(const ElementType*)&SrcElement.ElementData);
					}
					else
					{
						DestElement.PrevFreeIndex = SrcElement.PrevFreeIndex;
						DestElement.NextFreeIndex = SrcElement.NextFreeIndex;
					}
				}
			}
			else
			{
				// Use the much faster path for types that allow it
				FMemory::Memcpy(Data.GetData(), InCopy.Data.GetData(), sizeof(FElementOrFreeListLink) * SrcMax);
			}
		}
		return *this;
	}

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
		NE_ASSERT(Index >= 0);
		NE_ASSERT(Index < GetMaxIndex());
		NE_ASSERT(!AllocationFlags[Index]);

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

	FSparseArrayAllocationInfo AddUninitializedAtLowestFreeIndex(int32& LowestFreeIndexSearchStart)
	{
		int32 Index;
		if (NumFreeIndices)
		{
			Index = AllocationFlags.FindAndSetFirstZeroBit(LowestFreeIndexSearchStart);
			LowestFreeIndexSearchStart = Index + 1;

			auto& IndexData = GetData(Index);

			// Update FirstFreeIndex
			if (FirstFreeIndex == Index)
			{
				FirstFreeIndex = IndexData.NextFreeIndex;
			}

			// Link our next and prev free nodes together
			if (IndexData.NextFreeIndex >= 0)
			{
				GetData(IndexData.NextFreeIndex).PrevFreeIndex = IndexData.PrevFreeIndex;
			}

			if (IndexData.PrevFreeIndex >= 0)
			{
				GetData(IndexData.PrevFreeIndex).NextFreeIndex = IndexData.NextFreeIndex;
			}

			--NumFreeIndices;
		}
		else
		{
			// Add a new element.
			Index = Data.AddUninitialized(1);
			AllocationFlags.Add(true);
		}

		FSparseArrayAllocationInfo Result;
		Result.Index = Index;
		Result.Pointer = &GetData(Result.Index).ElementData;
		return Result;
	}

	template <typename... ArgsType>
	FORCEINLINE int32 Emplace(ArgsType&&... Args)
	{
		FSparseArrayAllocationInfo Allocation = AddUninitialized();
		new(Allocation) ElementType(Forward<ArgsType>(Args)...);
		return Allocation.Index;
	}

	template <typename... ArgsType>
	FORCEINLINE int32 EmplaceAtLowestFreeIndex(int32& LowestFreeIndexSearchStart, ArgsType&&... Args)
	{
		FSparseArrayAllocationInfo Allocation = AddUninitializedAtLowestFreeIndex(LowestFreeIndexSearchStart);
		new(Allocation) ElementType(Forward<ArgsType>(Args)...);
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
		return Data.GetData()[Index];
	}

	/** Accessor for the element or free list data. */
	const FElementOrFreeListLink& GetData(int32 Index) const
	{
		return Data.GetData()[Index];
	}

	// Accessors.
	ElementType& operator[](int32 Index)
	{
		NE_ASSERT(Index >= 0 && Index < Data.Num() && Index < AllocationFlags.Num());
		//checkSlow(AllocationFlags[Index]); // Disabled to improve loading times -BZ
		return *(ElementType*)&GetData(Index).ElementData;
	}
	const ElementType& operator[](int32 Index) const
	{
		NE_ASSERT(Index >= 0 && Index < Data.Num() && Index < AllocationFlags.Num());
		//checkSlow(AllocationFlags[Index]); // Disabled to improve loading times -BZ
		return *(ElementType*)&GetData(Index).ElementData;
	}

	int32 GetMaxIndex() const
	{
		return Data.Num();
	}

	int32 Num() const { return Data.Num() - NumFreeIndices; }

	/**
	 * Helper function to return the amount of memory allocated by this container
	 * Only returns the size of allocations made directly by the container, not the elements themselves.
	 * @return number of bytes allocated by this container
	 */
	uint32 GetAllocatedSize() const
	{
		return	(Data.Num() + Data.GetSlack()) * sizeof(FElementOrFreeListLink) +
			AllocationFlags.GetAllocatedSize();
	}

	bool IsValidIndex(int32 Index) const
	{
		return AllocationFlags.IsValidIndex(Index) && AllocationFlags[Index];
	}

	bool IsAllocated(int32 Index) const { return AllocationFlags[Index]; }

	/** Removes Count elements from the array, starting from Index. */
	void RemoveAt(int32 Index, int32 Count = 1)
	{
		if (!TIsTriviallyDestructible<ElementType>::Value)
		{
			for (int32 It = Index, ItCount = Count; ItCount; ++It, --ItCount)
			{
				((ElementType&)GetData(It).ElementData).~ElementType();
			}
		}

		RemoveAtUninitialized(Index, Count);
	}

	/** Removes Count elements from the array, starting from Index, without destructing them. */
	void RemoveAtUninitialized(int32 Index, int32 Count = 1)
	{
		for (; Count; --Count)
		{
			NE_ASSERT(AllocationFlags[Index]);

			// Mark the element as free and add it to the free element list.
			if (NumFreeIndices)
			{
				GetData(FirstFreeIndex).PrevFreeIndex = Index;
			}
			FElementOrFreeListLink& IndexData = GetData(Index);
			IndexData.PrevFreeIndex = -1;
			IndexData.NextFreeIndex = NumFreeIndices > 0 ? FirstFreeIndex : INDEX_NONE;
			FirstFreeIndex = Index;
			++NumFreeIndices;
			AllocationFlags[Index] = false;

			++Index;
		}
	}

	void Reset()
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
		Data.Reset();
		FirstFreeIndex = -1;
		NumFreeIndices = 0;
		AllocationFlags.Reset();
	}

	void Reserve(int32 ExpectedNumElements)
	{
		if (ExpectedNumElements > Data.Num())
		{
			const int32 ElementsToAdd = ExpectedNumElements - Data.Num();

			// allocate memory in the array itself
			int32 ElementIndex = Data.AddUninitialized(ElementsToAdd);

			// now mark the new elements as free
			for (int32 FreeIndex = ExpectedNumElements - 1; FreeIndex >= ElementIndex; --FreeIndex)
			{
				if (NumFreeIndices)
				{
					GetData(FirstFreeIndex).PrevFreeIndex = FreeIndex;
				}
				GetData(FreeIndex).PrevFreeIndex = -1;
				GetData(FreeIndex).NextFreeIndex = NumFreeIndices > 0 ? FirstFreeIndex : INDEX_NONE;
				FirstFreeIndex = FreeIndex;
				++NumFreeIndices;
			}

			if (ElementsToAdd == ExpectedNumElements)
			{
				AllocationFlags.Init(false, ElementsToAdd);
			}
			else
			{
				AllocationFlags.Add(false, ElementsToAdd);
			}
		}
	}

	/** Shrinks the array's storage to avoid slack. */
	void Shrink()
	{
		// Determine the highest allocated index in the data array.
		int32 MaxAllocatedIndex = AllocationFlags.FindLast(true);

		const int32 FirstIndexToRemove = MaxAllocatedIndex + 1;
		if (FirstIndexToRemove < Data.Num())
		{
			if (NumFreeIndices > 0)
			{
				// Look for elements in the free list that are in the memory to be freed.
				int32 FreeIndex = FirstFreeIndex;
				while (FreeIndex != INDEX_NONE)
				{
					if (FreeIndex >= FirstIndexToRemove)
					{
						const int32 PrevFreeIndex = GetData(FreeIndex).PrevFreeIndex;
						const int32 NextFreeIndex = GetData(FreeIndex).NextFreeIndex;
						if (NextFreeIndex != -1)
						{
							GetData(NextFreeIndex).PrevFreeIndex = PrevFreeIndex;
						}
						if (PrevFreeIndex != -1)
						{
							GetData(PrevFreeIndex).NextFreeIndex = NextFreeIndex;
						}
						else
						{
							FirstFreeIndex = NextFreeIndex;
						}
						--NumFreeIndices;

						FreeIndex = NextFreeIndex;
					}
					else
					{
						FreeIndex = GetData(FreeIndex).NextFreeIndex;
					}
				}
			}

			// Truncate unallocated elements at the end of the data array.
			Data.RemoveAt(FirstIndexToRemove, Data.Num() - FirstIndexToRemove);
			AllocationFlags.RemoveAt(FirstIndexToRemove, AllocationFlags.Num() - FirstIndexToRemove);
		}

		// Shrink the data array.
		Data.Shrink();
	}


	/** Compacts the allocated elements into a contiguous index range. */
	/** Returns true if any elements were relocated, false otherwise. */
	bool Compact()
	{
		int32 NumFree = NumFreeIndices;
		if (NumFree == 0)
		{
			return false;
		}

		bool bResult = false;

		FElementOrFreeListLink* ElementData = Data.GetData();

		int32 EndIndex = Data.Num();
		int32 TargetIndex = EndIndex - NumFree;
		int32 FreeIndex = FirstFreeIndex;
		while (FreeIndex != -1)
		{
			int32 NextFreeIndex = GetData(FreeIndex).NextFreeIndex;
			if (FreeIndex < TargetIndex)
			{
				// We need an element here
				do
				{
					--EndIndex;
				} while (!AllocationFlags[EndIndex]);

				RelocateConstructItems<FElementOrFreeListLink>(ElementData + FreeIndex, ElementData + EndIndex, 1);
				AllocationFlags[FreeIndex] = true;

				bResult = true;
			}

			FreeIndex = NextFreeIndex;
		}

		Data.RemoveAt(TargetIndex, NumFree);
		AllocationFlags.RemoveAt(TargetIndex, NumFree);

		NumFreeIndices = 0;
		FirstFreeIndex = -1;

		return bResult;
	}

	/** Sorts the elements using the provided comparison class. */
	template<typename PREDICATE_CLASS>
	void Sort(const PREDICATE_CLASS& Predicate)
	{
		if (Num() > 0)
		{
			// Compact the elements array so all the elements are contiguous.
			Compact();

			// Sort the elements according to the provided comparison class.
			Algo::IntroSort(&GetData(0), Num(), FElementCompareClass< PREDICATE_CLASS >(Predicate));
		}
	}

	/** Sorts the elements assuming < operator is defined for ElementType. */
	void Sort()
	{
		Sort(TLess< ElementType >());
	}
private:
	FORCEINLINE void Move(TSparseArray& ToArray, TSparseArray& FromArray)
	{
		// Destruct the allocated elements.
		if (!TIsTriviallyDestructible<ElementType>::Value)
		{
			for (ElementType& Element : ToArray)
			{
				DestructItem(&Element);
			}
		}

		ToArray.Data = (TArray<FElementOrFreeListLink>&&)FromArray.Data;
		ToArray.AllocationFlags = (FBitArray&&)FromArray.AllocationFlags;

		ToArray.FirstFreeIndex = FromArray.FirstFreeIndex;
		ToArray.NumFreeIndices = FromArray.NumFreeIndices;
		FromArray.FirstFreeIndex = -1;
		FromArray.NumFreeIndices = 0;
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

		TConstIterator(const TSparseArray& InArray, const FConstSetBitIterator& InBitArrayIt)
			: TBaseIterator<true>(InArray, InBitArrayIt)
		{
		}
	};

	/**
	* DO NOT USE DIRECTLY
	* STL-like iterators to enable range-based for loop support.
	*/
	FORCEINLINE TIterator      begin()       { return TIterator(*this); }
	FORCEINLINE TConstIterator begin() const { return TConstIterator(*this, FConstSetBitIterator(AllocationFlags)); }
	FORCEINLINE TIterator      end()         { return TIterator(*this, FConstSetBitIterator(AllocationFlags, AllocationFlags.Num())); }
	FORCEINLINE TConstIterator end()   const { return TConstIterator(*this, FConstSetBitIterator(AllocationFlags, AllocationFlags.Num())); }

private:

	/** Extracts the element value from the array's element structure and passes it to the user provided comparison class. */
	template <typename PREDICATE_CLASS>
	class FElementCompareClass
	{
		const PREDICATE_CLASS& Predicate;

	public:
		FElementCompareClass(const PREDICATE_CLASS& InPredicate)
			: Predicate(InPredicate)
		{}

		bool operator()(const FElementOrFreeListLink& A, const FElementOrFreeListLink& B) const
		{
			return Predicate(*(ElementType*)&A.ElementData, *(ElementType*)&B.ElementData);
		}
	};


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