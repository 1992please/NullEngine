#pragma once
#include "Core.h"


struct FMemory
{
	static void* Memmove(void* Dest, const void* Src, size_t Count);

	static int32 Memcmp(const void* Buf1, const void* Buf2, size_t Count);

	static void* Memset(void* Dest, uint8 Char, size_t Count);

	static void* Memzero(void* Dest, size_t Count);

	static void* Memcpy(void* Dest, const void* Src, size_t Count);

	static void* Malloc(size_t Count);

	static void* Realloc(void* Original, size_t Count);

	static void Free(void* Original);
};