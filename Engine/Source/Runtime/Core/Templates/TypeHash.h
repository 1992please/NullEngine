#pragma once

#include "CoreTypes.h"
#include "IsEnum.h"
#include "Misc/Crc.h"



/**
 * Combines two hash values to get a third.
 * Note - this function is not commutative.
 */
	inline uint32 HashCombine(uint32 A, uint32 C)
{
	uint32 B = 0x9e3779b9;
	A += B;

	A -= B; A -= C; A ^= (C >> 13);
	B -= C; B -= A; B ^= (A << 8);
	C -= A; C -= B; C ^= (B >> 13);
	A -= B; A -= C; A ^= (C >> 12);
	B -= C; B -= A; B ^= (A << 16);
	C -= A; C -= B; C ^= (B >> 5);
	A -= B; A -= C; A ^= (C >> 3);
	B -= C; B -= A; B ^= (A << 10);
	C -= A; C -= B; C ^= (B >> 15);

	return C;
}

inline uint32 PointerHash(const void* Key, uint32 C = 0)
{
	// Ignoring the lower 4 bits since they are likely zero anyway.
	// Higher bits are more significant in 64 bit builds.
	uint64 PtrInt = reinterpret_cast<uint64>(Key) >> 4;

	return HashCombine((uint32)PtrInt, C);
}

//
// Hash functions for common types.
//

inline uint32 GetTypeHash(const uint8 A)
{
	return A;
}

inline uint32 GetTypeHash(const int8 A)
{
	return A;
}

inline uint32 GetTypeHash(const uint16 A)
{
	return A;
}

inline uint32 GetTypeHash(const int16 A)
{
	return A;
}

inline uint32 GetTypeHash(const int32 A)
{
	return A;
}

inline uint32 GetTypeHash(const uint32 A)
{
	return A;
}

inline uint32 GetTypeHash(const uint64 A)
{
	return (uint32)A + ((uint32)(A >> 32) * 23);
}

inline uint32 GetTypeHash(const int64 A)
{
	return (uint32)A + ((uint32)(A >> 32) * 23);
}

inline uint32 GetTypeHash(float Value)
{
	return *(uint32*)&Value;
}

inline uint32 GetTypeHash(double Value)
{
	return GetTypeHash(*(uint64*)&Value);
}

inline uint32 GetTypeHash(const char* S)
{
	return FCrc::StrCrc32(S);
}

inline uint32 GetTypeHash(const void* A)
{
	return PointerHash(A);
}

inline uint32 GetTypeHash(void* A)
{
	return PointerHash(A);
}

template <typename EnumType>
FORCEINLINE  typename TEnableIf<TIsEnum<EnumType>::Value, uint32>::Type GetTypeHash(EnumType E)
{
	return GetTypeHash((__underlying_type(EnumType))E);
}