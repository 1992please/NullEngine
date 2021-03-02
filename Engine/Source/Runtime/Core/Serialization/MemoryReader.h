#pragma once
#include "Core/CoreTypes.h"
#include "Core/Serialization/MemoryArchive.h"

class FMemoryReader : public FMemoryArchive
{
public:
	explicit FMemoryReader(const TArray<uint8>& InBytes)
		: Bytes(InBytes)
	{
		ArIsLoading = 1;
	}

	virtual int32 TotalSize() override
	{
		return Bytes.Num();
	}

	void Serialize(void* Data, int32 Num)
	{
		if (Num && !ArIsError)
		{
			// Only serialize if we have the requested amount of data
			if (Offset + Num <= TotalSize())
			{
				FMemory::Memcpy(Data, &Bytes[(int32)Offset], Num);
				Offset += Num;
			}
			else
			{
				ArIsError = 1;
			}
		}
	}

private:
	const TArray<uint8>& Bytes;
};