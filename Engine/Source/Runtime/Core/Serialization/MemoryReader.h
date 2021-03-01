#pragma once
#include "Core/CoreTypes.h"
#include "Core/Serialization/MemoryReader.h"

class FMemoryReader : public FMemoryArchive
{
	explicit FMemoryReader(const TArray<uint8>& InBytes, bool bIsPersistent = false)
		: Bytes(InBytes)
		, ArIsLoading(1)
	{

	}

	virtual int64 TotalSize() override
	{
		return (int64)Bytes.Num();
	}

	void Serialize(void* Data, int64 Num)
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