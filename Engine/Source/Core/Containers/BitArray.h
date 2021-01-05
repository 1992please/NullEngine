#pragma once
#include "Core.h"
#include "Math/NullMath.h"
#include "NullMemory.h"

#define NumBitsPerDWORD ((uint32)32)
#define NumBitsPerDWORDLogTwo ((int32)5)

class FBitArray;

/** Used to reference a bit in an unspecified bit array. */
class FRelativeBitReference
{
public:
	FORCEINLINE explicit FRelativeBitReference(int32 BitIndex)
		: DWORDIndex(BitIndex >> NumBitsPerDWORDLogTwo)
		, Mask(1 << (BitIndex & (NumBitsPerDWORD - 1)))
	{
	}

	int32  DWORDIndex;
	uint32 Mask;
};

/** Used to read/write a bit in the array as a bool. */
class FBitReference
{
public:

	FORCEINLINE FBitReference(uint32& InData, uint32 InMask)
		: Data(InData)
		, Mask(InMask)
	{}

	FORCEINLINE operator bool() const
	{
		return (Data & Mask) != 0;
	}

	FORCEINLINE void operator=(const bool NewValue)
	{
		if (NewValue)
		{
			Data |= Mask;
		}
		else
		{
			Data &= ~Mask;
		}
	}

	FORCEINLINE void operator|=(const bool NewValue)
	{
		if (NewValue)
		{
			Data |= Mask;
		}
	}
	FORCEINLINE void operator&=(const bool NewValue)
	{
		if (!NewValue)
		{
			Data &= ~Mask;
		}
	}
	FORCEINLINE FBitReference& operator=(const FBitReference& Copy)
	{
		// As this is emulating a reference, assignment should not rebind,
		// it should write to the referenced bit.
		*this = (bool)Copy;
		return *this;
	}

private:
	uint32& Data;
	uint32 Mask;
};

class FBitArray
{
	typedef uint32 WordType;
	static constexpr WordType FullWordMask = (WordType)-1;
	static constexpr uint32 InitialNumOfWrods = 4;
public:
	FBitArray()
		: NumBits(0)
		, MaxBits(InitialNumOfWrods * NumBitsPerDWORD)
	{
		ResizeAllocation(InitialNumOfWrods, sizeof(uint32));
		// ClearPartialSlackBits is already satisfied since final word does not exist when NumBits == 0
	}

	FORCEINLINE explicit FBitArray(bool bValue, int32 InNumBits)
		: MaxBits(InitialNumOfWrods * NumBitsPerDWORD)
	{
		ResizeAllocation(InitialNumOfWrods, sizeof(uint32));
		Init(bValue, InNumBits);
	}

	FORCEINLINE FBitArray(FBitArray&& Other)
	{
		Move(*this, Other);
	}

	FORCEINLINE FBitArray(const FBitArray& Copy)
		: NumBits(0)
		, MaxBits(0)
	{
		Assign(Copy);
	}

	FORCEINLINE FBitArray& operator=(FBitArray&& Other)
	{
		if (this != &Other)
		{
			Move(*this, Other);
		}

		return *this;
	}

	FBitArray& operator=(const FBitArray& Copy)
	{
		// check for self assignment since we don't use swap() mechanic
		if (this != &Copy)
		{
			Assign(Copy);
		}
		return *this;
	}

	FORCEINLINE bool operator==(const FBitArray& Other) const
	{
		if (Num() != Other.Num())
		{
			return false;
		}

		return FMemory::Memcmp(GetData(), Other.GetData(), GetNumWords() * sizeof(uint32)) == 0;
	}

	FORCEINLINE bool operator<(const FBitArray& Other) const
	{
		//sort by length
		if (Num() != Other.Num())
		{
			return Num() < Other.Num();
		}

		uint32 NumWords = GetNumWords();
		const uint32* Data0 = GetData();
		const uint32* Data1 = Other.GetData();

		//lexicographically compare
		for (uint32 i = 0; i < NumWords; i++)
		{
			if (Data0[i] != Data1[i])
			{
				return Data0[i] < Data1[i];
			}
		}
		return false;
	}

	FORCEINLINE bool operator!=(const FBitArray& Other) const
	{
		return !(*this == Other);
	}


	FORCEINLINE const uint32* GetData() const
	{
		return Data;
	}

	FORCEINLINE uint32* GetData()
	{
		return Data;
	}

	int32 Add(const bool Value)
	{
		const int32 Index = AddUninitialized(1);
		SetBitNoCheck(Index, Value);
		return Index;
	}

	int32 Add(const bool Value, int32 NumBitsToAdd)
	{
		// Support for legacy behavior requires us to silently ignore NumBitsToAdd < 0
		if (NumBitsToAdd < 0)
		{
			return NumBits;
		}
		const int32 Index = AddUninitialized(NumBitsToAdd);
		SetRange(Index, NumBitsToAdd, Value);
		return Index;
	}

	int32 AddUninitialized(int32 NumBitsToAdd)
	{
		ASSERT(NumBitsToAdd >= 0);
		int32 AddedIndex = NumBits;
		if (NumBitsToAdd > 0)
		{
			int32 OldLastWordIndex = NumBits == 0 ? -1 : (NumBits - 1) / NumBitsPerDWORD;
			int32 NewLastWordIndex = (NumBits + NumBitsToAdd - 1) / NumBitsPerDWORD;
			if (NewLastWordIndex == OldLastWordIndex)
			{
				// We're not extending into a new word, so we don't need to reserve more memory and we don't need to clear the unused bits on the final word
				NumBits += NumBitsToAdd;
			}
			else
			{
				Reserve(NumBits + NumBitsToAdd);
				NumBits += NumBitsToAdd;
				ClearPartialSlackBits();
			}
		}
		return AddedIndex;
	}

	void Reserve(int32 Number)
	{
		if (Number > MaxBits)
		{
			const uint32 MaxDWORDs = CalculateNumWords(Number);
			MaxBits = MaxDWORDs * NumBitsPerDWORD;
			Realloc();
		}
	}

	void Reset()
	{
		NumBits = 0;
	}

	FORCEINLINE void Init(bool bValue, int32 InNumBits)
	{
		NumBits = InNumBits;

		const uint32 NumWords = GetNumWords();
		const uint32 MaxWords = GetMaxWords();

		if (NumWords > 0)
		{
			if (NumWords > MaxWords)
			{
				ResizeAllocation(NumWords, sizeof(uint32));
				MaxBits = NumWords * NumBitsPerDWORD;
			}

			SetWords(GetData(), NumWords, bValue);
			ClearPartialSlackBits();
		}
	}

	FORCENOINLINE void SetRange(int32 Index, int32 NumBitsToSet, bool Value)
	{
		ASSERT(Index >= 0 && NumBitsToSet >= 0 && Index + NumBitsToSet <= NumBits);

		if (NumBitsToSet == 0)
		{
			return;
		}

		// Work out which uint32 index to set from, and how many
		uint32 StartIndex = Index / NumBitsPerDWORD;
		uint32 Count = (Index + NumBitsToSet + (NumBitsPerDWORD - 1)) / NumBitsPerDWORD - StartIndex;

		// Work out masks for the start/end of the sequence
		uint32 StartMask = FullWordMask << (Index % NumBitsPerDWORD);
		uint32 EndMask = FullWordMask >> (NumBitsPerDWORD - (Index + NumBitsToSet) % NumBitsPerDWORD) % NumBitsPerDWORD;

		uint32* Data = GetData() + StartIndex;
		if (Value)
		{
			if (Count == 1)
			{
				*Data |= StartMask & EndMask;
			}
			else
			{
				*Data++ |= StartMask;
				Count -= 2;
				while (Count != 0)
				{
					*Data++ = ~0;
					--Count;
				}
				*Data |= EndMask;
			}
		}
		else
		{
			if (Count == 1)
			{
				*Data &= ~(StartMask & EndMask);
			}
			else
			{
				*Data++ &= ~StartMask;
				Count -= 2;
				while (Count != 0)
				{
					*Data++ = 0;
					--Count;
				}
				*Data &= ~EndMask;
			}
		}

		CheckInvariants();
	}

	/**
	 * Removes all bits from the array, potentially leaving space allocated for an expected number of bits about to be added.
	 * @param ExpectedNumBits - The expected number of bits about to be added.
	 */
	void Empty(int32 ExpectedNumBits = 0)
	{
		ExpectedNumBits = static_cast<int32>(CalculateNumWords(ExpectedNumBits)) * NumBitsPerDWORD;
		const int32 InitialMaxBits = InitialNumOfWrods * NumBitsPerDWORD;

		NumBits = 0;

		// If we need more bits or can shrink our allocation, do so
		// Otherwise, reuse current initial allocation
		if (ExpectedNumBits > MaxBits || MaxBits > InitialMaxBits)
		{
			MaxBits = FMath::Max(ExpectedNumBits, InitialMaxBits);
			Realloc();
		}
	}

	FORCEINLINE int32 Num() const { return NumBits; }
	FORCEINLINE int32 Max() const { return MaxBits; }

	FORCEINLINE FBitReference operator[](int32 Index)
	{
		ASSERT(Index >= 0 && Index < NumBits);
		return FBitReference(
			GetData()[Index / NumBitsPerDWORD],
			1 << (Index & (NumBitsPerDWORD - 1))
		);
	}

	void CheckInvariants() const
	{
#if _DEBUG
		ASSERT(NumBits <= MaxBits && NumBits >= 0 && MaxBits >= 0);

		// Verify the ClearPartialSlackBits invariant
		const int32 UsedBits = (NumBits % NumBitsPerDWORD);
		if (UsedBits != 0)
		{
			const int32 LastDWORDIndex = NumBits / NumBitsPerDWORD;
			const uint32 SlackMask = FullWordMask << UsedBits;

			const uint32 LastDWORD = *(GetData() + LastDWORDIndex);
			ASSERT((LastDWORD & SlackMask) == 0);
		}
#endif
	}

private:
	FORCEINLINE uint32 GetNumWords() const
	{
		return CalculateNumWords(NumBits);
	}

	FORCEINLINE uint32 GetMaxWords() const
	{
		return CalculateNumWords(MaxBits);
	}

	FORCEINLINE static void SetWords(uint32* Words, int32 NumWords, bool bValue)
	{
		if (NumWords > 8)
		{
			FMemory::Memset(Words, bValue ? 0xff : 0, NumWords * sizeof(uint32));
		}
		else
		{
			uint32 Word = bValue ? ~0u : 0u;
			for (int32 Idx = 0; Idx < NumWords; ++Idx)
			{
				Words[Idx] = Word;
			}
		}
	}

	FORCEINLINE void ResizeAllocation(int32 NumElements, size_t NumBytesPerElement)
	{
		// Avoid calling FMemory::Realloc( nullptr, 0 ) as ANSI C mandates returning a valid pointer which is not what we want.
		if (Data || NumElements)
		{
			//checkSlow(((uint64)NumElements*(uint64)ElementTypeInfo.GetSize() < (uint64)INT_MAX));
			Data = (uint32*)FMemory::Realloc(Data, NumElements*NumBytesPerElement);
		}
	}


	static FORCEINLINE void Move(FBitArray& ToArray, FBitArray& FromArray)
	{
		ASSERT(&ToArray != &FromArray);

		if (ToArray.Data)
		{
			FMemory::Free(ToArray.Data);
		}

		ToArray.Data = FromArray.Data;
		FromArray.Data = nullptr;

		ToArray.NumBits = FromArray.NumBits;
		ToArray.MaxBits = FromArray.MaxBits;
		FromArray.NumBits = 0;
		FromArray.MaxBits = 0;
		// No need to call this.ClearPartialSlackBits, because the words we're copying or moving from satisfy the invariant
		// No need to call FromArray.ClearPartialSlackBits because NumBits == 0 automatically satisfies the invariant
	}

	void Assign(const FBitArray& Other)
	{
		Empty(Other.Num());
		NumBits = Other.Num();
		if (NumBits)
		{
			FMemory::Memcpy(GetData(), Other.GetData(), GetNumWords() * sizeof(uint32));
			// No need to call ClearPartialSlackBits, because the bits we're copying from satisfy the invariant
			// If NumBits == 0, the invariant is automatically satisfied so we also don't need ClearPartialSlackBits in that case.
		}
	}

	/**
	 * Clears the slack bits within the final partially relevant DWORD
	 */
	void ClearPartialSlackBits()
	{
		// FBitArray has a contract about bits outside of the active range - the bits in the final word past NumBits are guaranteed to be 0
		// This prevents easy-to-make determinism errors from users of FBitArray that do not carefully mask the final word.
		// It also allows us optimize some operations which would otherwise require us to mask the last word.
		const int32 UsedBits = NumBits % NumBitsPerDWORD;
		if (UsedBits != 0)
		{
			const int32  LastDWORDIndex = NumBits / NumBitsPerDWORD;
			const uint32 SlackMask = FullWordMask >> (NumBitsPerDWORD - UsedBits);

			uint32* LastDWORD = (GetData() + LastDWORDIndex);
			*LastDWORD = *LastDWORD & SlackMask;
		}
	}

	FORCEINLINE static uint32 CalculateNumWords(int32 NumBits)
	{
		ASSERT(NumBits >= 0);
		return FMath::DivideAndRoundUp(static_cast<uint32>(NumBits), NumBitsPerDWORD);
	}

	static FORCEINLINE uint32 GetAndClearNextBit(uint32& Mask)
	{
		const uint32 LowestBitMask = (Mask) & (-(int32)Mask);
		const uint32 BitIndex = FMath::FloorLog2(LowestBitMask);
		Mask ^= LowestBitMask;
		return BitIndex;
	}

	FORCENOINLINE void Realloc()
	{
		const uint32 MaxDWORDs = CalculateNumWords(MaxBits);

		ResizeAllocation(MaxDWORDs, sizeof(uint32));
		ClearPartialSlackBits(); // Implement class invariant
	}

	void SetBitNoCheck(int32 Index, bool Value)
	{
		uint32& Word = GetData()[Index / NumBitsPerDWORD];
		uint32 BitOffset = (Index % NumBitsPerDWORD);
		Word = (Word & ~(1 << BitOffset)) | (((uint32)Value) << BitOffset);
	}

private:

	uint32*       Data;
	int32         NumBits;
	int32         MaxBits;

};

/** An iterator which only iterates over set bits. */
class FConstSetBitIterator : public FRelativeBitReference
{
public:

	/** Constructor. */
	FConstSetBitIterator(const FBitArray& InArray, int32 StartIndex = 0)
		: FRelativeBitReference(StartIndex)
		, Array(InArray)
		, UnvisitedBitMask((~0U) << (StartIndex & (NumBitsPerDWORD - 1)))
		, CurrentBitIndex(StartIndex)
		, BaseBitIndex(StartIndex & ~(NumBitsPerDWORD - 1))
	{
		ASSERT(StartIndex >= 0 && StartIndex <= Array.Num());
		if (StartIndex != Array.Num())
		{
			FindFirstSetBit();
		}
	}

	/** Forwards iteration operator. */
	FORCEINLINE FConstSetBitIterator& operator++()
	{
		// Mark the current bit as visited.
		UnvisitedBitMask &= ~Mask;

		// Find the first set bit that hasn't been visited yet.
		FindFirstSetBit();

		return *this;
	}

	FORCEINLINE friend bool operator==(const FConstSetBitIterator& Lhs, const FConstSetBitIterator& Rhs)
	{
		// We only need to compare the bit index and the array... all the rest of the state is unobservable.
		return Lhs.CurrentBitIndex == Rhs.CurrentBitIndex && &Lhs.Array == &Rhs.Array;
	}

	FORCEINLINE friend bool operator!=(const FConstSetBitIterator& Lhs, const FConstSetBitIterator& Rhs)
	{
		return !(Lhs == Rhs);
	}

	/** conversion to "bool" returning true if the iterator is valid. */
	FORCEINLINE explicit operator bool() const
	{
		return CurrentBitIndex < Array.Num();
	}
	/** inverse of the "bool" operator */
	FORCEINLINE bool operator !() const
	{
		return !(bool)*this;
	}

	/** Index accessor. */
	FORCEINLINE int32 GetIndex() const
	{
		return CurrentBitIndex;
	}

private:

	const FBitArray& Array;

	uint32 UnvisitedBitMask;
	int32 CurrentBitIndex;
	int32 BaseBitIndex;

	/** Find the first set bit starting with the current bit, inclusive. */
	void FindFirstSetBit()
	{
		const uint32* ArrayData = Array.GetData();
		const int32   ArrayNum = Array.Num();
		const int32   LastDWORDIndex = (ArrayNum - 1) / NumBitsPerDWORD;

		// Advance to the next non-zero uint32.
		uint32 RemainingBitMask = ArrayData[DWORDIndex] & UnvisitedBitMask;
		while (!RemainingBitMask)
		{
			++this->DWORDIndex;
			BaseBitIndex += NumBitsPerDWORD;
			if (DWORDIndex > LastDWORDIndex)
			{
				// We've advanced past the end of the array.
				CurrentBitIndex = ArrayNum;
				return;
			}

			RemainingBitMask = ArrayData[DWORDIndex];
			UnvisitedBitMask = ~0;
		}

		// This operation has the effect of unsetting the lowest set bit of BitMask
		const uint32 NewRemainingBitMask = RemainingBitMask & (RemainingBitMask - 1);

		// This operation XORs the above mask with the original mask, which has the effect
		// of returning only the bits which differ; specifically, the lowest bit
		Mask = NewRemainingBitMask ^ RemainingBitMask;

		// If the Nth bit was the lowest set bit of BitMask, then this gives us N
		CurrentBitIndex = BaseBitIndex + NumBitsPerDWORD - 1 - FMath::CountLeadingZeros(Mask);

		// If we've accidentally iterated off the end of an array but still within the same DWORD
		// then set the index to the last index of the array
		if (CurrentBitIndex > ArrayNum)
		{
			CurrentBitIndex = ArrayNum;
		}
	}
};