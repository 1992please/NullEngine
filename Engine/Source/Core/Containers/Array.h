#pragma once
#include "Core.h"
#include "Math/NumericLimits.h"
#include "Templates/IsTriviallyDestructible.h"
#include "Templates/TypeTraits.h"
#include "Templates/NullTemplate.h"
#include "Templates/Sorting.h"
#include "Templates/MemoryOps.h"

#include "Algo/HeapSort.h"

#include "NullMemory.h"
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
	{}

	FORCEINLINE TArray(const ElementType* Ptr, int32 Count)
	{
		ASSERT(Ptr != nullptr || Count == 0);

		CopyToEmpty(Ptr, Count, 0, 0);
	}

	FORCEINLINE TArray(const TArray& Other)
	{
		CopyToEmpty(Other.Data, Other.Num(), 0, 0);
	}

	FORCEINLINE TArray(const TArray& Other, int32 ExtraSlack)
	{
		CopyToEmpty(Other.Data, Other.Num(), 0, ExtraSlack);
	}

	FORCEINLINE TArray(TArray<ElementType>&& Other)
	{
		Move(*this, Other);
	}

	/**
	 * Initializer list constructor
	 */
	TArray(std::initializer_list<InElementType> InitList)
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
		ASSERT((ArrayNum >= 0) & (ArrayMax >= ArrayNum)); // & for one branch
	}

	FORCEINLINE void RangeCheck(int32 Index) const
	{
		CheckInvariants();

		ASSERT((Index >= 0) & (Index < ArrayNum)); // & for one branch
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
		ASSERT(Count >= 0);

		const int32 OldNum = ArrayNum;
		if ((ArrayNum += Count) > ArrayMax)
		{
			ResizeGrow(OldNum);
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
		ASSERT(Addr < Data || Addr >= (Data + ArrayMax));
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

		ASSERT(Slack >= 0);
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
		const ElementType* __restrict Start = Data;
		for (const ElementType* __restrict Data = Start, *__restrict DataEnd = Data + ArrayNum; Data != DataEnd; ++Data)
		{
			if (*Data == Item)
			{
				return static_cast<int32>(Data - Start);
			}
		}
		return INDEX_NONE;
	}

	FORCEINLINE int32 AddUnique(ElementType&& Item) { return AddUniqueImpl(MoveTempIfPossible(Item)); }
	FORCEINLINE int32 AddUnique(const ElementType& Item) { return AddUniqueImpl(Item); }

	FORCEINLINE void Reserve(int32 Number)
	{
		ASSERT(Number >= 0);
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

	int32 Remove(const ElementType& Item)
	{
		CheckAddress(&Item);

		// Element is non-const to preserve compatibility with existing code with a non-const operator==() member function
		return RemoveAll([&Item](ElementType& Element) { return Element == Item; });
	}

	template <typename OtherElementType>
	void Append(const TArray<OtherElementType>& Source)
	{
		ASSERT((void*)this != (void*)&Source);

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
		ASSERT((void*)this != (void*)&Source);

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
		ASSERT(Ptr != nullptr || Count == 0);

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

	/**
	 * Performs heap sort on the array. Assumes < operator is defined for the
	 * template type.
	 *
	 * @note: If your array contains raw pointers, they will be automatically dereferenced during heapification.
	 *        Therefore, your array will be heapified by the values being pointed to, rather than the pointers' values.
	 *        The auto-dereferencing behavior does not occur with smart pointers.
	 */
	void HeapSort()
	{
		HeapSort(TLess<ElementType>());
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
		ASSERT(ExtraSlack >= 0);

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
			ResizeAllocation(0, NewMax, sizeof(ElementType));
			ArrayMax = NewMax;
		}
		else
		{
			ArrayMax = PrevMax;
		}
	}

	FORCEINLINE void ResizeAllocation(int32 PreviousNumElements, int32 NumElements, size_t NumBytesPerElement)
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
			ASSERT((Count >= 0) & (Index >= 0) & (Index + Count <= ArrayNum));

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

	FORCENOINLINE void ResizeGrow(int32 OldNum)
	{
		//const SIZE_T FirstGrow = 1;
		//const SIZE_T ConstantGrow = 0;
		const int32 FirstGrow = 4;
		const int32 ConstantGrow = 16;

		ASSERT(ArrayNum > ArrayMax && ArrayNum > 0);

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

		ResizeAllocation(OldNum, ArrayMax, sizeof(ElementType));
	}

	FORCENOINLINE void ResizeShrink()
	{
		int32 NewArrayMax;
		{
			ASSERT(ArrayNum < ArrayMax);
			// If the container has too much slack, shrink it to exactly fit the number of elements.
			const size_t BytesPerElement = sizeof(ElementType);
			const int32 CurrentSlackElements = ArrayMax - ArrayNum;
			const size_t CurrentSlackBytes = (ArrayMax - ArrayNum)*BytesPerElement;
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
			ASSERT(ArrayMax >= ArrayNum);
			ResizeAllocation(ArrayNum, ArrayMax, sizeof(ElementType));
		}
	}

	FORCENOINLINE void ResizeTo(int32 NewMax)
	{
		ASSERT(NewMax > 0);
		if (NewMax != ArrayMax)
		{
			ArrayMax = NewMax;
			ResizeAllocation(ArrayNum, ArrayMax, sizeof(ElementType));
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
			ASSERT(RunLength > 0);
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

protected:
	int32				ArrayNum;
	int32				ArrayMax;
	ElementType*		Data;
};