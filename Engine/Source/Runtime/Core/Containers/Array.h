#pragma once
#include "Core/CoreTypes.h"
#include "Core/Math/NumericLimits.h"
#include "Core/Templates/IsTriviallyDestructible.h"
#include "Core/Templates/TypeTraits.h"
#include "Core/Templates/NullTemplate.h"
#include "Core/Templates/Sorting.h"
#include "Core/Templates/MemoryOps.h"

#include "Core/Algo/IntroSort.h"
#include "Core/Algo/HeapSort.h"

#include "Core/Assert/Assert.h"

#include "Core/Memory/NullMemory.h"
#include <initializer_list>


template<typename InElementType>
class TArray
{
	template <typename OtherInElementType>
	friend class TArray;

public:
	typedef InElementType ElementType;

	FORCEINLINE TArray()
		: ArrayNum(0)
		, ArrayMax(0)
		, Data(nullptr)
	{}

	FORCEINLINE TArray(const ElementType* Ptr, int32 Count) 
		: Data(nullptr)
	{
		NE_ASSERT(Ptr != nullptr || Count == 0);

		CopyToEmpty(Ptr, Count, 0, 0);
	}

	FORCEINLINE TArray(const TArray& Other)
		: Data(nullptr)
	{
		CopyToEmpty(Other.Data, Other.Num(), 0, 0);
	}

	FORCEINLINE TArray(const TArray& Other, int32 ExtraSlack)
		: Data(nullptr)
	{
		CopyToEmpty(Other.Data, Other.Num(), 0, ExtraSlack);
	}

	FORCEINLINE TArray(TArray<ElementType>&& Other)
		: Data(nullptr)
	{
		Move(*this, Other);
	}

	/**
	 * Initializer list constructor
	 */
	TArray(std::initializer_list<InElementType> InitList)
		: Data(nullptr)
	{
		// This is not strictly legal, as std::initializer_list's iterators are not guaranteed to be pointers, but
		// this appears to be the case on all of our implementations.  Also, if it's not true on a new implementation,
		// it will fail to compile rather than behave badly.
		CopyToEmpty(InitList.begin(), (int32)InitList.size(), 0, 0);
	}

	template <typename OtherElementType>
	FORCEINLINE explicit TArray(const TArray<OtherElementType>& Other)
	{
		CopyToEmpty(Other.Data, Other.Num(), 0, 0);
	}

	TArray& operator=(const TArray& Other)
	{
		if (this != &Other)
		{
			DestructItems(Data, ArrayNum);
			CopyToEmpty(Other.Data, Other.Num(), ArrayMax, 0);
		}
		return *this;
	}

	TArray& operator=(std::initializer_list<InElementType> InitList)
	{
		DestructItems(Data, ArrayNum);
		// This is not strictly legal, as std::initializer_list's iterators are not guaranteed to be pointers, but
		// this appears to be the case on all of our implementations.  Also, if it's not true on a new implementation,
		// it will fail to compile rather than behave badly.
		CopyToEmpty(InitList.begin(), (SizeType)InitList.size(), ArrayMax, 0);
		return *this;
	}

	TArray& operator=(TArray&& Other)
	{
		if (this != &Other)
		{
			DestructItems(Data, ArrayNum);
			Move(*this, Other);
		}
		return *this;
	}

	~TArray()
	{
		DestructItems(Data, ArrayNum);
		if (Data)
		{
			FMemory::Free(Data);
			Data = nullptr;
		}
	}

	FORCEINLINE ElementType* GetData()
	{
		return Data;
	}

	FORCEINLINE const ElementType* GetData() const
	{
		return Data;
	}

	FORCEINLINE int32 GetTypeSize() const
	{
		return sizeof(ElementType);
	}

	FORCEINLINE size_t GetAllocatedSize() const
	{
		return ArrayMax * sizeof(ElementType);
	}

	FORCEINLINE int32 GetSlack() const
	{
		return ArrayMax - ArrayNum;
	}

	FORCEINLINE void CheckInvariants() const
	{
		NE_ASSERT((ArrayNum >= 0) & (ArrayMax >= ArrayNum)); // & for one branch
	}

	FORCEINLINE void RangeCheck(int32 Index) const
	{
		CheckInvariants();

		NE_ASSERT((Index >= 0) & (Index < ArrayNum)); // & for one branch
	}

	FORCEINLINE bool IsValidIndex(int32 Index) const
	{
		return Index >= 0 && Index < ArrayNum;
	}

	FORCEINLINE int32 Num() const
	{
		return ArrayNum;
	}

	FORCEINLINE int32 Max() const
	{
		return ArrayMax;
	}

	FORCEINLINE ElementType& operator[](int32 Index)
	{
		RangeCheck(Index);
		return Data[Index];
	}

	FORCEINLINE const ElementType& operator[](int32 Index) const
	{
		RangeCheck(Index);
		return Data[Index];
	}

	FORCEINLINE int32 AddUninitialized(int32 Count = 1)
	{
		CheckInvariants();
		NE_ASSERT(Count >= 0);

		const int32 OldNum = ArrayNum;
		if ((ArrayNum += Count) > ArrayMax)
		{
			ResizeGrow();
		}
		return OldNum;
	}

	template <typename... ArgsType>
	FORCEINLINE int32 Emplace(ArgsType&&... Args)
	{
		const int32 Index = AddUninitialized(1);
		new(Data + Index) ElementType(Forward<ArgsType>(Args)...);
		return Index;
	}

	FORCEINLINE void CheckAddress(const ElementType* Addr) const
	{
		NE_ASSERT(Addr < Data || Addr >= (Data + ArrayMax));
	}

	FORCEINLINE int32 Add(const ElementType& Item)
	{
		CheckAddress(&Item);
		return Emplace(Item);
	}

	FORCEINLINE int32 Add(ElementType&& Item)
	{
		CheckAddress(&Item);
		return Emplace(MoveTempIfPossible(Item));
	}

	FORCEINLINE ElementType Pop(bool bAllowShrinking = true)
	{
		RangeCheck(0);
		ElementType Result = MoveTempIfPossible(Data[ArrayNum - 1]);
		RemoveAt(ArrayNum - 1, 1, bAllowShrinking);
		return Result;
	}

	FORCEINLINE void Push(ElementType&& Item)
	{
		Add(MoveTempIfPossible(Item));
	}

	FORCEINLINE void Push(const ElementType& Item)
	{
		Add(Item);
	}

	FORCEINLINE void RemoveAt(int32 Index)
	{
		RemoveAtImpl(Index, 1, true);
	}

	FORCEINLINE void RemoveAt(int32 Index, int32 Count, bool bAllowShrinking = true)
	{
		RemoveAtImpl(Index, Count, bAllowShrinking);
	}

	FORCEINLINE void Shrink()
	{
		CheckInvariants();
		if (ArrayMax != ArrayNum)
		{
			ResizeTo(ArrayNum);
		}
	}

	FORCEINLINE void Reset(int32 NewSize = 0)
	{
		// If we have space to hold the excepted size, then don't reallocate
		if (NewSize <= ArrayMax)
		{
			DestructItems(Data, ArrayNum);
			ArrayNum = 0;
		}
		else
		{
			Empty(NewSize);
		}
	}

	void Empty(int32 Slack = 0)
	{
		DestructItems(Data, ArrayNum);

		NE_ASSERT(Slack >= 0);
		ArrayNum = 0;

		if (ArrayMax != Slack)
		{
			ResizeTo(Slack);
		}
	}

	void SetNum(int32 NewNum, bool bAllowShrinking = true)
	{
		if (NewNum > Num())
		{
			const int32 Diff = NewNum - ArrayNum;
			const int32 Index = AddUninitialized(Diff);
			DefaultConstructItems<ElementType>((uint8*)Data + Index * sizeof(ElementType), Diff);
		}
		else if (NewNum < Num())
		{
			RemoveAt(NewNum, Num() - NewNum, bAllowShrinking);
		}
	}

	FORCEINLINE ElementType& Top()
	{
		return Last();
	}

	FORCEINLINE const ElementType& Top() const
	{
		return Last();
	}

	FORCEINLINE ElementType& Last(int32 IndexFromTheEnd = 0)
	{
		RangeCheck(ArrayNum - IndexFromTheEnd - 1);
		return Data[ArrayNum - IndexFromTheEnd - 1];
	}

	FORCEINLINE const ElementType& Last(int32 IndexFromTheEnd = 0) const
	{
		RangeCheck(ArrayNum - IndexFromTheEnd - 1);
		return Data[ArrayNum - IndexFromTheEnd - 1];
	}

	FORCEINLINE bool Find(const ElementType& Item, int32& Index) const
	{
		Index = Find(Item);
		return Index != INDEX_NONE;
	}

	int32 Find(const ElementType& Item) const
	{
		const ElementType* RESTRICT Start = Data;
		for (const ElementType* RESTRICT pData = Start, *RESTRICT DataEnd = pData + ArrayNum; pData != DataEnd; ++pData)
		{
			if (*pData == Item)
			{
				return static_cast<int32>(pData - Start);
			}
		}
		return INDEX_NONE;
	}

	FORCEINLINE int32 AddUnique(ElementType&& Item) { return AddUniqueImpl(MoveTempIfPossible(Item)); }
	FORCEINLINE int32 AddUnique(const ElementType& Item) { return AddUniqueImpl(Item); }

	FORCEINLINE void Reserve(int32 Number)
	{
		NE_ASSERT(Number >= 0);
		if (Number > ArrayMax)
		{
			ResizeTo(Number);
		}
	}

	void Init(const ElementType& Element, int32 Number)
	{
		Empty(Number);
		for (int32 Index = 0; Index < Number; ++Index)
		{
			new(*this) ElementType(Element);
		}
	}

	int32 RemoveSingle(const ElementType& Item)
	{
		int32 Index = Find(Item);
		if (Index == INDEX_NONE)
		{
			return 0;
		}

		ElementType* RemovePtr = GetData() + Index;

		// Destruct items that match the specified Item.
		DestructItems(RemovePtr, 1);
		const int32 NextIndex = Index + 1;
		//RelocateConstructItems<ElementType>(RemovePtr, RemovePtr + 1, ArrayNum - (Index + 1));
		FMemory::Memmove(RemovePtr, RemovePtr + 1, sizeof(ElementType)* ArrayNum - (Index + 1));
		// Update the array count
		--ArrayNum;

		// Removed one item
		return 1;
	}

	int32 Remove(const ElementType& Item)
	{
		CheckAddress(&Item);

		// Element is non-const to preserve compatibility with existing code with a non-const operator==() member function
		return RemoveAll([&Item](ElementType& Element) { return Element == Item; });
	}

	template <class PREDICATE_CLASS>
	int32 RemoveAll(const PREDICATE_CLASS& Predicate)
	{
		const int32 OriginalNum = ArrayNum;
		if (!OriginalNum)
		{
			return 0; // nothing to do, loop assumes one item so need to deal with this edge case here
		}

		int32 WriteIndex = 0;
		int32 ReadIndex = 0;
		bool NotMatch = !Predicate(Data[ReadIndex]); // use a ! to guarantee it can't be anything other than zero or one
		do
		{
			int32 RunStartIndex = ReadIndex++;
			while (ReadIndex < OriginalNum && NotMatch == !Predicate(Data[ReadIndex]))
			{
				ReadIndex++;
			}
			int32 RunLength = ReadIndex - RunStartIndex;
			NE_ASSERT(RunLength > 0);
			if (NotMatch)
			{
				// this was a non-matching run, we need to move it
				if (WriteIndex != RunStartIndex)
				{
					FMemory::Memmove(&Data[WriteIndex], &Data[RunStartIndex], sizeof(ElementType)* RunLength);
				}
				WriteIndex += RunLength;
			}
			else
			{
				// this was a matching run, delete it
				DestructItems(Data + RunStartIndex, RunLength);
			}
			NotMatch = !NotMatch;
		} while (ReadIndex < OriginalNum);

		ArrayNum = WriteIndex;
		return OriginalNum - ArrayNum;
	}

	/**
	 * Removes item from the array.
	 *
	 * This version is much more efficient, because it uses RemoveAtSwap
	 * internally which is O(Count) instead of RemoveAt which is O(ArrayNum),
	 * but does not preserve the order.
	 *
	 * @returns Number of elements removed.
	 * @see Add, Insert, Remove, RemoveAll, RemoveAllSwap
	 */
	int32 RemoveSwap(const ElementType& Item, bool bAllowShrinking = true)
	{
		CheckAddress(&Item);

		const int32 OriginalNum = ArrayNum;
		bool bRemoved = false;
		for (int32 Index = 0; Index < ArrayNum; Index++)
		{
			if ((*this)[Index] == Item)
			{
				bRemoved = true;
				RemoveAtSwap(Index--, 1, false);
			}
		}

		if (bRemoved && bAllowShrinking)
		{
			ResizeShrink();
		}

		return OriginalNum - ArrayNum;
	}

	/**
	 * Removes an element (or elements) at given location optionally shrinking
	 * the array.
	 *
	 * This version is much more efficient than RemoveAt (O(Count) instead of
	 * O(ArrayNum)), but does not preserve the order.
	 *
	 * @param Index Location in array of the element to remove.
	 * @param Count (Optional) Number of elements to remove. Default is 1.
	 * @param bAllowShrinking (Optional) Tells if this call can shrink array if
	 *                        suitable after remove. Default is true.
	 */
	FORCEINLINE void RemoveAtSwap(int32 Index, int32 Count = 1, bool bAllowShrinking = true)
	{
		RemoveAtSwapImpl(Index, Count, bAllowShrinking);
	}

	template <typename OtherElementType>
	void Append(const TArray<OtherElementType>& Source)
	{
		NE_ASSERT((void*)this != (void*)&Source);

		int32 SourceCount = Source.Num();

		// Do nothing if the source is empty.
		if (!SourceCount)
		{
			return;
		}

		// Allocate memory for the new elements.
		Reserve(ArrayNum + SourceCount);
		ConstructItems<ElementType>(Data + ArrayNum, Source.Data, SourceCount);

		ArrayNum += SourceCount;
	}

	template <typename OtherElementType>
	void Append(TArray<OtherElementType>&& Source)
	{
		NE_ASSERT((void*)this != (void*)&Source);

		int32 SourceCount = Source.Num();

		// Do nothing if the source is empty.
		if (!SourceCount)
		{
			return;
		}

		// Allocate memory for the new elements.
		Reserve(ArrayNum + SourceCount);
		RelocateConstructItems<ElementType>(Data + ArrayNum, Source.Data, SourceCount);
		Source.ArrayNum = 0;

		ArrayNum += SourceCount;
	}

	void Append(const ElementType* Ptr, int32 Count)
	{
		NE_ASSERT(Ptr != nullptr || Count == 0);

		int32 Pos = AddUninitialized(Count);
		ConstructItems<ElementType>(Data + Pos, Ptr, Count);
	}

	FORCEINLINE void Append(std::initializer_list<ElementType> InitList)
	{
		int32 Count = (int32)InitList.size();

		int32 Pos = AddUninitialized(Count);
		ConstructItems<ElementType>(Data + Pos, InitList.begin(), Count);
	}

	TArray& operator+=(TArray&& Other)
	{
		Append(MoveTemp(Other));
		return *this;
	}

	TArray& operator+=(const TArray& Other)
	{
		Append(Other);
		return *this;
	}

	TArray& operator+=(std::initializer_list<ElementType> InitList)
	{
		Append(InitList);
		return *this;
	}

	/**
 * Finds an element which matches a predicate functor.
 *
 * @param Pred The functor to apply to each element.
 * @returns Pointer to the first element for which the predicate returns true, or nullptr if none is found.
 * @see FilterByPredicate, ContainsByPredicate
 */
	template <class Predicate>
	FORCEINLINE const ElementType* FindByPredicate(Predicate Pred) const
	{
		return const_cast<TArray*>(this)->FindByPredicate(Pred);
	}

	/**
	 * Finds an element which matches a predicate functor.
	 *
	 * @param Pred The functor to apply to each element. true, or nullptr if none is found.
	 * @see FilterByPredicate, ContainsByPredicate
	 */
	template <class Predicate>
	ElementType* FindByPredicate(Predicate Pred)
	{
		for (ElementType* RESTRICT pData = Data, *RESTRICT DataEnd = pData + ArrayNum; pData != DataEnd; ++pData)
		{
			if (Pred(*pData))
			{
				return pData;
			}
		}

		return nullptr;
	}

	/**
	 * Filters the elements in the array based on a predicate functor.
	 *
	 * @param Pred The functor to apply to each element.
	 * @returns TArray with the same type as this object which contains
	 *          the subset of elements for which the functor returns true.
	 * @see FindByPredicate, ContainsByPredicate
	 */
	template <class Predicate>
	TArray<ElementType> FilterByPredicate(Predicate Pred) const
	{
		TArray<ElementType> FilterResults;
		for (const ElementType* RESTRICT pData = GetData(), *RESTRICT DataEnd = pData + ArrayNum; pData != DataEnd; ++pData)
		{
			if (Pred(*pData))
			{
				FilterResults.Add(*pData);
			}
		}
		return FilterResults;
	}

	/**
	 * Checks if this array contains the element.
	 *
	 * @returns	True if found. False otherwise.
	 * @see ContainsByPredicate, FilterByPredicate, FindByPredicate
	 */
	template <typename ComparisonType>
	bool Contains(const ComparisonType& Item) const
	{
		for (const ElementType* RESTRICT pData = GetData(), *RESTRICT DataEnd = pData + ArrayNum; pData != DataEnd; ++pData)
		{
			if (*pData == Item)
			{
				return true;
			}
		}
		return false;
	}

	/**
	 * Checks if this array contains element for which the predicate is true.
	 *
	 * @param Predicate to use
	 * @returns	True if found. False otherwise.
	 * @see Contains, Find
	 */
	template <class Predicate>
	FORCEINLINE bool ContainsByPredicate(Predicate Pred) const
	{
		return FindByPredicate(Pred) != nullptr;
	}

	/**
	 * Performs heap sort on the array.
	 *
	 * @param Predicate Predicate class instance.
	 *
	 * @note: If your array contains raw pointers, they will be automatically dereferenced during heapification.
	 *        Therefore, your predicate will be passed references rather than pointers.
	 *        The auto-dereferencing behavior does not occur with smart pointers.
	 */
	template <class PREDICATE_CLASS>
	void HeapSort(const PREDICATE_CLASS& Predicate)
	{
		TDereferenceWrapper<ElementType, PREDICATE_CLASS> PredicateWrapper(Predicate);
		Algo::HeapSort(Data, ArrayNum, PredicateWrapper);
	}

	void HeapSort()
	{
		HeapSort(TLess<ElementType>());
	}

	void Sort()
	{
		Sort(TLess<ElementType>());
	}

	template <class PREDICATE_CLASS>
	void Sort(const PREDICATE_CLASS& Predicate)
	{
		TDereferenceWrapper<ElementType, PREDICATE_CLASS> PredicateWrapper(Predicate);
		Algo::IntroSort(Data, ArrayNum, PredicateWrapper);
	}

	void Insert(const ElementType* Ptr, int32 Count, int32 Index)
	{
		NE_ASSERT(Ptr != nullptr);

		InsertUninitializedImpl(Index, Count);
		ConstructItems<ElementType>(GetData() + Index, Ptr, Count);
	}

	void Insert(ElementType&& Item, int32 Index)
	{
		CheckAddress(&Item);

		// construct a copy in place at Index (this new operator will insert at 
		// Index, then construct that memory with Item)
		InsertUninitializedImpl(Index, 1);
		new(GetData() + Index) ElementType(MoveTempIfPossible(Item));
	}

	void Insert(const ElementType& Item, int32 Index)
	{
		CheckAddress(&Item);

		// construct a copy in place at Index (this new operator will insert at 
		// Index, then construct that memory with Item)
		InsertUninitializedImpl(Index, 1);
		new(GetData() + Index) ElementType(Item);
	}

	/**
	* DO NOT USE DIRECTLY
	* STL-like iterators to enable range-based for loop support.
	*/
	FORCEINLINE ElementType*           begin()       { return GetData(); }
	FORCEINLINE const ElementType*     begin() const { return GetData(); }
	FORCEINLINE ElementType*           end()         { return GetData() + Num(); }
	FORCEINLINE const ElementType*     end() const   { return GetData() + Num(); }

private:

	template <typename OtherElementType>
	void CopyToEmpty(const OtherElementType* OtherData, int32 NewNum, int32 PrevMax, int32 ExtraSlack)
	{
		NE_ASSERT(ExtraSlack >= 0);

		ArrayNum = NewNum;
		if (NewNum || ExtraSlack || PrevMax)
		{
			ResizeForCopy(NewNum + ExtraSlack, PrevMax);
			ConstructItems<ElementType>(Data, OtherData, NewNum);
		}
		else
		{
			ArrayMax = 0;
		}
	}

	FORCENOINLINE void ResizeForCopy(int32 NewMax, int32 PrevMax)
	{
		if (NewMax > PrevMax)
		{
			ResizeAllocation(NewMax, sizeof(ElementType));
			ArrayMax = NewMax;
		}
		else
		{
			ArrayMax = PrevMax;
		}
	}

	FORCEINLINE void ResizeAllocation(int32 NumElements, size_t NumBytesPerElement)
	{
		// Avoid calling FMemory::Realloc( nullptr, 0 ) as ANSI C mandates returning a valid pointer which is not what we want.
		if (Data || NumElements)
		{
			//checkSlow(((uint64)NumElements*(uint64)ElementTypeInfo.GetSize() < (uint64)INT_MAX));
			Data = (ElementType*)FMemory::Realloc(Data, NumElements*NumBytesPerElement);
		}
	}

	FORCEINLINE void Move(TArray<ElementType>& ToArray, TArray<ElementType>& FromArray)
	{
		ToArray.ArrayNum = FromArray.ArrayNum;
		ToArray.ArrayMax = FromArray.ArrayMax;
		FromArray.ArrayNum = 0;
		FromArray.ArrayMax = 0;


		if (ToArray.Data)
			FMemory::Free(ToArray.Data);

		ToArray.Data = FromArray.Data;
		FromArray.Data = nullptr;
	}

	void RemoveAtImpl(int32 Index, int32 Count, bool bAllowShrinking)
	{
		if (Count)
		{
			CheckInvariants();
			NE_ASSERT((Count >= 0) & (Index >= 0) & (Index + Count <= ArrayNum));

			DestructItems(Data + Index, Count);

			// Skip memmove in the common case that there is nothing to move.
			int32 NumToMove = ArrayNum - Index - Count;
			if (NumToMove)
			{
				FMemory::Memmove
				(
					(uint8*)Data + (Index) * sizeof(ElementType),
					(uint8*)Data + (Index + Count) * sizeof(ElementType),
					NumToMove * sizeof(ElementType)
				);
			}
			ArrayNum -= Count;

			if (bAllowShrinking)
			{
				ResizeShrink();
			}
		}
	}

	FORCENOINLINE void ResizeGrow()
	{
		//const SIZE_T FirstGrow = 1;
		//const SIZE_T ConstantGrow = 0;
		const int32 FirstGrow = 4;
		const int32 ConstantGrow = 16;

		NE_ASSERT(ArrayNum > ArrayMax && ArrayNum > 0);

		int32 Grow = FirstGrow; // this is the amount for the first alloc

		if (ArrayMax)
		{
			// Allocate slack for the array proportional to its size.
			Grow = ArrayNum + 3 * ArrayNum / 8 + ConstantGrow;
		}
		else if (ArrayNum > Grow)
		{
			Grow = ArrayNum;
		}
		// NumElements and MaxElements are stored in 32 bit signed integers so we must be careful not to overflow here.
		if (ArrayNum > Grow)
		{
			Grow = TNumericLimits<int32>::Max();
		}

		ArrayMax = Grow;

		ResizeAllocation(ArrayMax, sizeof(ElementType));
	}

	FORCENOINLINE void ResizeShrink()
	{
		int32 NewArrayMax;
		{
			NE_ASSERT(ArrayNum < ArrayMax);
			// If the container has too much slack, shrink it to exactly fit the number of elements.
			const size_t BytesPerElement = sizeof(ElementType);
			const int32 CurrentSlackElements = ArrayMax - ArrayNum;
			const size_t CurrentSlackBytes = size_t(ArrayMax - ArrayNum) * BytesPerElement;
			const bool bTooManySlackBytes = CurrentSlackBytes >= 16384;
			const bool bTooManySlackElements = 3 * ArrayNum < 2 * ArrayMax;
			if ((bTooManySlackBytes || bTooManySlackElements) && (CurrentSlackElements > 64 || !ArrayNum)) //  hard coded 64 :-(
			{
				NewArrayMax = ArrayNum;
			}
			else
			{
				NewArrayMax = ArrayMax;
			}
		}
		
		if (NewArrayMax != ArrayMax)
		{
			ArrayMax = NewArrayMax;
			NE_ASSERT(ArrayMax >= ArrayNum);
			ResizeAllocation(ArrayMax, sizeof(ElementType));
		}
	}

	FORCENOINLINE void ResizeTo(int32 NewMax)
	{
		if (NewMax != ArrayMax)
		{
			ArrayMax = NewMax;
			ResizeAllocation(ArrayMax, sizeof(ElementType));
		}
	}

	template <typename ArgsType>
	int32 AddUniqueImpl(ArgsType&& Args)
	{
		int32 Index;
		if (Find(Args, Index))
		{
			return Index;
		}

		return Add(Forward<ArgsType>(Args));
	}

	void RemoveAtSwapImpl(int32 Index, int32 Count = 1, bool bAllowShrinking = true)
	{
		if (Count)
		{
			CheckInvariants();
			NE_ASSERT((Count >= 0) & (Index >= 0) & (Index + Count <= ArrayNum));

			DestructItems(GetData() + Index, Count);

			// Replace the elements in the hole created by the removal with elements from the end of the array, so the range of indices used by the array is contiguous.
			const int32 NumElementsInHole = Count;
			const int32 NumElementsAfterHole = ArrayNum - (Index + Count);
			const int32 NumElementsToMoveIntoHole = FMath::Min(NumElementsInHole, NumElementsAfterHole);
			if (NumElementsToMoveIntoHole)
			{
				FMemory::Memcpy(
					(uint8*)Data + (Index) * sizeof(ElementType),
					(uint8*)Data + (ArrayNum - NumElementsToMoveIntoHole) * sizeof(ElementType),
					NumElementsToMoveIntoHole * sizeof(ElementType)
				);
			}
			ArrayNum -= Count;

			if (bAllowShrinking)
			{
				ResizeShrink();
			}
		}
	}

	void InsertUninitializedImpl(int32 Index, int32 Count)
	{
		CheckInvariants();
		NE_ASSERT((Count >= 0) & (Index >= 0) & (Index <= ArrayNum));

		const int32 OldNum = ArrayNum;
		if ((ArrayNum += Count) > ArrayMax)
		{
			ResizeGrow();
		}
		ElementType* Data = GetData() + Index;
		RelocateConstructItems<ElementType>(Data + Count, Data, OldNum - Index);
	}


protected:
	int32				ArrayNum;
	int32				ArrayMax;
	ElementType*		Data;
};