#include "BitArray.h"

void FBitArrayMemory::MemmoveBitsWordOrder(uint32* StartDest, int32 DestOffset, const uint32* StartSource, int32 SourceOffset, uint32 NumBits)
{
	// Normalize Inputs
	NE_ASSERT(NumBits >= 0);
	if (NumBits <= 0)
	{
		return;
	}
	ModularizeWordOffset(StartDest, DestOffset);
	ModularizeWordOffset(StartSource, SourceOffset);

	// If the Dest words are aligned with the Source Words, use the aligned version of MemmoveBits which requires fewer masking instructions.
	// This also allows us to reduce complexity by not needing to handle the aligned case in the rest of this function.
	const int32 DeltaOffset = DestOffset - SourceOffset;
	if (DeltaOffset == 0)
	{
		MemmoveBitsWordOrderAlignedInternal(StartDest, StartSource, DestOffset, NumBits);
		return;
	}

}

void FBitArrayMemory::MemmoveBitsWordOrderAlignedInternal(uint32*const StartDest, const uint32*const StartSource, int32 StartOffset, uint32 NumBits)
{
	// Contracts guaranteed by caller:
	// NumBits > 0
	// 0 <= StartOffset < NumBitsPerDWORD

	// Calculate Starting and EndingMasks
	const int32 EndOffset = (StartOffset + NumBits - 1) % NumBitsPerDWORD + 1;
	const uint32 StartMask = MAX_uint32 << StartOffset;
	const uint32 EndMask = MAX_uint32 >> (NumBitsPerDWORD - EndOffset);

	// Calculate Start and Final Pointers
	const uint32 OffsetToLastWord = (StartOffset + NumBits - 1) / NumBitsPerDWORD;
	uint32* const FinalDest = StartDest + OffsetToLastWord;
	const uint32* const FinalSource = StartSource + OffsetToLastWord;

	// If the end word is the start word, use special handling to apply both the start mask and the end mask
	if (OffsetToLastWord == 0)
	{
		uint32 CombinedMask = StartMask & EndMask;
		*StartDest = (*StartDest & ~CombinedMask) | (*StartSource & CombinedMask);
		return;
	}

	// If the Dest range and Source range overlap, we need to dest the words of dest in the same order as the direction from deststart to sourcestart, so that words are read from the destrange before being overwritten
	// If Dest is less than Source, this is handled by forward iteration, which is the default since it is better for cache coherency
	// If Source is less than Dest AND they overlap, we need to iterate backwards
	const bool bRequiresBackward = (StartSource < StartDest) & (StartDest <= FinalSource);
	if (!bRequiresBackward)
	{
		*StartDest = (*StartDest & ~StartMask) | (*StartSource & StartMask);
		uint32* CurrentDest = StartDest + 1;
		const uint32* CurrentSource = StartSource + 1;
		while (CurrentDest < FinalDest)
		{
			*CurrentDest++ = *CurrentSource++;
		}
		*FinalDest = (*FinalSource & EndMask) | (*FinalDest & ~EndMask);
	}
	else
	{
		*FinalDest = (*FinalSource & EndMask) | (*FinalDest & ~EndMask);
		uint32* CurrentDest = FinalDest - 1;
		const uint32* CurrentSource = FinalSource - 1;
		while (CurrentDest > StartDest)
		{
			*CurrentDest-- = *CurrentSource--;
		}
		*StartDest = (*StartDest & ~StartMask) | (*StartSource & StartMask);
	}
}


void FBitArrayMemory::ModularizeWordOffset(uint32 const*& Data, int32& Offset)
{
	if ((Offset < 0) | (NumBitsPerDWORD <= Offset))
	{
		int32 NumWords = Offset / NumBitsPerDWORD;
		Data += NumWords;
		Offset -= NumWords * NumBitsPerDWORD;
		if (Offset < 0)
		{
			--Data;
			Offset += NumBitsPerDWORD;
		}
	}
}
