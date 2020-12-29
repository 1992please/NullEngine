#pragma once
#include "Core.h"
#include "Math/NumericLimits.h"
#include "Templates/IsTriviallyDestructible.h"
#include "Templates/NullTypeTraits.h"
#include "Templates/NullTemplate.h"

#include "NullMemory.h"

template<typename InElementType>
class NArray
{
public:
	typedef InElementType ElementType;

	FORCEINLINE NArray()
		: ArrayNum(0)
		, ArrayMax(0)
	{}

	FORCEINLINE NArray(const ElementType* Ptr, int32 Count)
	{
		ASSERT(Ptr != nullptr || Count == 0);

		CopyToEmpty(Ptr, Count, 0, 0);
	}

	FORCEINLINE NArray(const NArray& Other)
	{
		CopyToEmpty(Other.Data, Other.Num(), 0, 0);
	}

	FORCEINLINE NArray(const NArray& Other, int32 ExtraSlack)
	{
		CopyToEmpty(Other.Data, Other.Num(), 0, ExtraSlack);
	}

	NArray& operator=(const NArray& Other)
	{
		if (this != &Other)
		{
			DestructItems(Data, ArrayNum);
			CopyToEmpty(Other.Data, Other.Num(), ArrayMax, 0);
		}
		return *this;
	}

	FORCEINLINE NArray(NArray<ElementType>&& Other)
	{
		Move(*this, Other);
	}

	NArray& operator=(NArray&& Other)
	{
		if (this != &Other)
		{
			DestructItems(Data, ArrayNum);
			Move(*this, Other);
		}
		return *this;
	}

	~NArray()
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
			DefaultConstructItems((uint8*)Data + Index * sizeof(ElementType), Diff);
		}
		else if (NewNum < Num())
		{
			RemoveAt(NewNum, Num() - NewNum, bAllowShrinking);
		}
	}

private:


	void CopyToEmpty(const ElementType* OtherData, int32 NewNum, int32 PrevMax, int32 ExtraSlack)
	{
		ASSERT(ExtraSlack >= 0);

		ArrayNum = NewNum;
		if (NewNum || ExtraSlack || PrevMax)
		{
			ResizeForCopy(NewNum + ExtraSlack, PrevMax);
			FMemory::Memcpy(Data, OtherData, sizeof(ElementType) * NewNum);
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

	FORCEINLINE void Move(NArray<ElementType>& ToArray, NArray<ElementType>& FromArray)
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

	/**
	* Destructs a range of items in memory.
	*
	* @param	Elements	A pointer to the first item to destruct.
	* @param	Count		The number of elements to destruct.
	*
	* @note: This function is optimized for values of T, and so will not dynamically dispatch destructor calls if T's destructor is virtual.
	*/

	FORCEINLINE void DestructItems(ElementType* Element, int32 Count)
	{
		if constexpr (!TIsTriviallyDestructible<ElementType>::Value)
		{
			while (Count)
			{
				// We need a typedef here because VC won't compile the destructor call below if ElementType itself has a member called ElementType
				//typedef ElementType DestructItemsElementTypeTypedef;

				Element->ElementType::~ElementType();
				++Element;
				--Count;
			}
		}
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

	FORCEINLINE void DefaultConstructItems(void* Address, int32 Count)
	{
		if constexpr (TIsZeroConstructType<ElementType>::Value)
		{
			FMemory::Memset(Address, 0, sizeof(ElementType) * Count);
		}
		else
		{
			ElementType* Element = (ElementType*)Address;
			while (Count)
			{
				new (Element) ElementType;
				++Element;
				--Count;
			}
		}
	}



protected:
	int32				ArrayNum;
	int32				ArrayMax;
	ElementType*		Data;
};