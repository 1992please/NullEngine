#pragma once
#include "Core/CoreTypes.h"
#include "Core/Serialization/MemoryArchive.h"

class FMemoryWriter : public FMemoryArchive
{
public:
	FMemoryWriter(TArray<uint8>& InBytes, bool bSetOffset = false)
		: Bytes(InBytes)
	{
		ArIsLoading = 0;
		if (bSetOffset)
		{
			Offset = InBytes.Num();
		}
	}

	virtual void Serialize(void* Data, int32 Num) override
	{
		const int32 NumBytesToAdd = Offset + Num - Bytes.Num();
		if (NumBytesToAdd > 0)
		{
			const int32 NewArrayCount = Bytes.Num() + NumBytesToAdd;
			NE_CHECK(NewArrayCount < MAX_int32);

			Bytes.AddUninitialized((int32)NumBytesToAdd);
		}

		NE_CHECK((Offset + Num) <= Bytes.Num());

		if (Num)
		{
			FMemory::Memcpy(&Bytes[(int32)Offset], Data, Num);
			Offset += Num;
		}
	}

	int32 TotalSize() override
	{
		return Bytes.Num();
	}

protected:

	TArray<uint8>&	Bytes;
};