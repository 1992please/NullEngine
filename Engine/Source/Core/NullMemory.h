#pragma once
#include "GlobalDefs.h"

struct FMemory
{
	static FORCEINLINE void* Memmove(void* Dest, const void* Src, size_t Count)
	{
		return memmove(Dest, Src, Count);
	}

	static FORCEINLINE int32 Memcmp(const void* Buf1, const void* Buf2, size_t Count)
	{
		return memcmp(Buf1, Buf2, Count);
	}

	static FORCEINLINE void* Memset(void* Dest, uint8 Char, size_t Count)
	{
		return memset(Dest, Char, Count);
	}

	static FORCEINLINE void* Memzero(void* Dest, size_t Count)
	{
		return memset(Dest, 0, Count);
	}

	static FORCEINLINE void* Memcpy(void* Dest, const void* Src, size_t Count)
	{
		return memcpy(Dest, Src, Count);
	}

	static FORCEINLINE void* Malloc(size_t Count)
	{
		return malloc(Count);
	}

	static FORCEINLINE void* Realloc(void* Original, size_t Count)
	{
		return realloc(Original, Count);
	}

	static FORCEINLINE void Free(void* Original)
	{
		free(Original);
	}
};