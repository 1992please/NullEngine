#pragma once
#include "NullMemory.h"

template <typename T>
struct TIsTriviallyDestructible
{
	enum { Value = __has_trivial_destructor(T) };
};

template<typename InElementType>
class NArray
{
public:
	typedef InElementType ElementType;

	FORCEINLINE NArray()
		: ArrayNum(0)
		, ArrayMax(0)
	{}

	FORCEINLINE NArray(const ElementType* Ptr, uint32 Count)
	{
		ASSERT(Ptr != nullptr || Count == 0);

		CopyToEmpty(Ptr, Count, 0, 0);
	}

	FORCEINLINE NArray(const NArray& Other)
	{
		CopyToEmpty(Other.GetData(), Other.Num(), 0, 0);
	}

	FORCEINLINE NArray(const NArray& Other, uint32 ExtraSlack)
	{
		CopyToEmpty(Other.GetData(), Other.Num(), 0, ExtraSlack);
	}

	NArray& operator=(const NArray& Other)
	{
		if (this != &Other)
		{
			DestructItems(GetData(), ArrayNum);
			CopyToEmpty(Other.GetData(), Other.Num(), ArrayMax, 0);
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
			DestructItems(GetData(), ArrayNum);
			Move(*this, Other);
		}
		return *this;
	}

	~NArray()
	{
		DestructItems(GetData(), ArrayNum);
	}

	FORCEINLINE ElementType* GetData()
	{
		return Data;
	}

	FORCEINLINE const ElementType* GetData() const
	{
		return Data;
	}

	FORCEINLINE uint32 GetTypeSize() const
	{
		return sizeof(ElementType);
	}

	FORCEINLINE size_t GetAllocatedSize() const
	{
		return ArrayMax * sizeof(ElementType);
	}

	FORCEINLINE uint32 GetSlack() const
	{
		return ArrayMax - ArrayNum;
	}

	FORCEINLINE void CheckInvariants() const
	{
		ASSERT((ArrayNum >= 0) & (ArrayMax >= ArrayNum)); // & for one branch
	}

	FORCEINLINE void RangeCheck(uint32 Index) const
	{
		CheckInvariants();

		ASSERT((Index >= 0) & (Index < ArrayNum)); // & for one branch
	}

	FORCEINLINE bool IsValidIndex(uint32 Index) const
	{
		return Index >= 0 && Index < ArrayNum;
	}

	FORCEINLINE uint32 Num() const
	{
		return ArrayNum;
	}

	FORCEINLINE uint32 Max() const
	{
		return ArrayMax;
	}

	FORCEINLINE ElementType& operator[](uint32 Index)
	{
		RangeCheck(Index);
		return GetData()[Index];
	}

	FORCEINLINE const ElementType& operator[](uint32 Index) const
	{
		RangeCheck(Index);
		return GetData()[Index];
	}

	FORCENOINLINE void ResizeGrow(uint32 OldNum)
	{
		//const SIZE_T FirstGrow = 1;
		//const SIZE_T ConstantGrow = 0;
		const uint32 FirstGrow = 4;
		const uint32 ConstantGrow = 16;

		ASSERT(ArrayNum > ArrayMax && ArrayNum > 0);

		uint32 Grow = FirstGrow; // this is the amount for the first alloc

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
			Grow = TNumericLimits<uint32>::Max();
		}

		ArrayMax = Grow;

		ResizeAllocation(OldNum, ArrayMax, sizeof(ElementType));
	}

	FORCEINLINE uint32 AddUninitialized(uint32 Count = 1)
	{
		CheckInvariants();
		ASSERT(Count >= 0);

		const uint32 OldNum = ArrayNum;
		if ((ArrayNum += Count) > ArrayMax)
		{
			ResizeGrow(OldNum);
		}
		return OldNum;
	}

	template <typename... ArgsType>
	FORCEINLINE uint32 Emplace(ArgsType&&... Args)
	{
		const uint32 Index = AddUninitialized(1);
		new(GetData() + Index) ElementType(Forward<ArgsType>(Args)...);
		return Index;
	}

	FORCEINLINE void CheckAddress(const ElementType* Addr) const
	{
		ASSERT(Addr < GetData() || Addr >= (GetData() + ArrayMax));
	}

	FORCEINLINE uint32 Add(const ElementType& Item)
	{
		CheckAddress(&Item);
		return Emplace(Item);
	}

	FORCEINLINE uint32 Add(ElementType&& Item)
	{
		CheckAddress(&Item);
		return Emplace(MoveTempIfPossible(Item));
	}

	//FORCEINLINE ElementType Pop(bool bAllowShrinking = true)
	//{
	//	RangeCheck(0);
	//	ElementType Result = MoveTempIfPossible(GetData()[ArrayNum - 1]);
	//	RemoveAt(ArrayNum - 1, 1, bAllowShrinking);
	//	return Result;
	//}

	FORCEINLINE void Push(ElementType&& Item)
	{
		Add(MoveTempIfPossible(Item));
	}

	FORCEINLINE void Push(const ElementType& Item)
	{
		Add(Item);
	}

	void CopyToEmpty(const ElementType* OtherData, uint32 NewNum, uint32 PrevMax, uint32 ExtraSlack)
	{
		ASSERT(ExtraSlack >= 0);

		ArrayNum = NewNum;
		if (NewNum || ExtraSlack || PrevMax)
		{
			ResizeForCopy(NewNum + ExtraSlack, PrevMax);
			FMemory::Memcpy(GetData(), OtherData, sizeof(ElementType) * NewNum);
		}
		else
		{
			ArrayMax = 0;
		}
	}

	FORCENOINLINE void ResizeForCopy(uint32 NewMax, uint32 PrevMax)
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

	FORCEINLINE void ResizeAllocation(uint32 PreviousNumElements, uint32 NumElements, size_t NumBytesPerElement)
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


		if(ToArray.Data)
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

	FORCEINLINE void DestructItems(ElementType* Element, uint32 Count)
	{
		if constexpr (!TIsTriviallyDestructible<ElementType>::Value)
		{
			while (Count)
			{
				// We need a typedef here because VC won't compile the destructor call below if ElementType itself has a member called ElementType
				typedef ElementType DestructItemsElementTypeTypedef;

				Element->DestructItemsElementTypeTypedef::~DestructItemsElementTypeTypedef();
				++Element;
				--Count;
			}
		}
	}

protected:
	uint32				ArrayNum;
	uint32				ArrayMax;
	ElementType*		Data;
};