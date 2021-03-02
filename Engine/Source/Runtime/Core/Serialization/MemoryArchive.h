#pragma once
#include "Core/CoreTypes.h"


class FMemoryArchive
{
public:
	virtual void Serialize(void* V, int32 Length) { }

	virtual int32 TotalSize()
	{
		return 0;
	}

	template<typename T>
	FORCEINLINE friend FMemoryArchive& operator<<(FMemoryArchive& Ar, T& Value)
	{
		Ar.Serialize(&Value, sizeof(T));
		return Ar;
	}

	FORCEINLINE friend FMemoryArchive& operator<<(FMemoryArchive& Ar, bool& D)
	{
		uint32 OldUBoolValue = D ? 1 : 0;
		Ar.Serialize(&OldUBoolValue, sizeof(OldUBoolValue));
		D = !!OldUBoolValue;
		return Ar;
	}

	FORCEINLINE bool IsLoading() const
	{
		return ArIsLoading;
	}

	FORCEINLINE bool IsError() const
	{
		return ArIsError;
	}

protected:
	FMemoryArchive()
		: Offset(0)
		, ArIsError(0)
	{
	}

	uint8 ArIsLoading;
	uint8 ArIsError;
	int32 Offset;
};