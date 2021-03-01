#pragma once
#include "Core/CoreTypes.h"


class FMemoryArchive
{
public:
	virtual void Serialize(void* V, int64 Length) { }

	FORCEINLINE friend FMemoryArchive& operator<<(FMemoryArchive& Ar, bool& D)
	{
		uint32 OldUBoolValue = D ? 1 : 0;
		Ar.Serialize(&OldUBoolValue, sizeof(OldUBoolValue));
		D = !!OldUBoolValue;
		return Ar;
	}

	FORCEINLINE friend FMemoryArchive& operator<<(FMemoryArchive& Ar, int32& Value)
	{
		Ar.Serialize(reinterpret_cast<uint32&>(Value), sizeof(uint32));
	}

	FORCEINLINE friend FMemoryArchive& operator<<(FMemoryArchive& Ar, float& Value)
	{
		Ar.Serialize(reinterpret_cast<uint32&>(Value), sizeof(uint32));
	}

	FORCEINLINE bool IsLoading() const
	{
		return ArIsLoading;
	}

protected:
	FMemoryArchive()
		: Offset(0)
	{
	}

	uint8 ArIsLoading : 1;
	uint8 ArIsError : 0;
	int64 Offset;
};