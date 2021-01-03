#pragma once
#include "Core.h"
#include "Math/NullMath.h"
#include "NullMemory.h"

#define NumBitsPerDWORD ((uint32)32)

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

	/**
	 * Minimal initialization constructor.
	 * @param Value - The value to initial the bits to.
	 * @param InNumBits - The initial number of bits in the array.
	 */
	FORCEINLINE explicit FBitArray(bool bValue, int32 InNumBits)
		: MaxBits(InitialNumOfWrods * NumBitsPerDWORD)
	{
		ResizeAllocation(InitialNumOfWrods, sizeof(uint32));
		Init(bValue, InNumBits);
	}

	/**
	 * Move constructor.
	 */
	FORCEINLINE FBitArray(FBitArray&& Other)
	{
		Move(*this, Other);
	}

	/**
	 * Copy constructor.
	 */
	FORCEINLINE FBitArray(const FBitArray& Copy)
		: NumBits(0)
		, MaxBits(0)
	{
		Assign(Copy);
	}

	FORCEINLINE const uint32* GetData() const
	{
		return Data;
	}

	FORCEINLINE uint32* GetData()
	{
		return Data;
	}

	/**
	 * Resets the array's contents. Use FBitArray(bool bValue, int32 InNumBits) instead of default constructor and Init().
	 *
	 * @param Value - The value to initial the bits to.
	 * @param NumBits - The number of bits in the array.
	 */
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

private:

	uint32*       Data;
	int32         NumBits;
	int32         MaxBits;

};