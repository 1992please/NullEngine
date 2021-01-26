#include "NullMemory.h"
#include <string.h>
#include <stdlib.h>

void* FMemory::Memmove(void* Dest, const void* Src, size_t Count)
{
	return memmove(Dest, Src, Count);
}

int32 FMemory::Memcmp(const void* Buf1, const void* Buf2, size_t Count)
{
	return memcmp(Buf1, Buf2, Count);
}

void* FMemory::Memset(void* Dest, uint8 Char, size_t Count)
{
	return memset(Dest, Char, Count);
}

void* FMemory::Memzero(void* Dest, size_t Count)
{
	return memset(Dest, 0, Count);
}

void* FMemory::Memcpy(void* Dest, const void* Src, size_t Count)
{
	return memcpy(Dest, Src, Count);
}

void* FMemory::Malloc(size_t Count)
{
	return malloc(Count);
}

void* FMemory::Realloc(void* Original, size_t Count)
{
	return realloc(Original, Count);
}

void FMemory::Free(void* Original)
{
	free(Original);
}


void* operator new(size_t size)
{
	return FMemory::Malloc(size);
}

void operator delete (void* mem)
{
	FMemory::Free(mem);
}

void* operator new[](size_t size)
{
	return FMemory::Malloc(size);
}

void operator delete[](void* mem)
{
	FMemory::Free(mem);
}
