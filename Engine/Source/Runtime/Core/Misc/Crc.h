#pragma once
#include "Core/CoreTypes.h"
/**
 * CRC hash generation for different types of input data
 **/
struct NE_API FCrc
{
	/** lookup table with precalculated CRC values - slicing by 8 implementation */
	static uint32 CRCTablesSB8[8][256];

	/** initializes the CRC lookup table. Must be called before any of the
	CRC functions are used. */
	static void CheckCrc();

	/** generates CRC hash of the memory area */
	static uint32 MemCrc32(const void* Data, int32 Length, uint32 CRC = 0);

	/** generates CRC hash of the element */
	template <typename T>
	static uint32 TypeCrc32(const T& Data, uint32 CRC = 0)
	{
		return MemCrc32(&Data, sizeof(T), CRC);
	}

	/** String CRC. */
	static uint32 StrCrc32(const char* Data, uint32 CRC = 0)
	{
		/* Overload for when CharType is a byte, which causes warnings when right-shifting by 8 */
		CRC = ~CRC;
		while (char Ch = *Data++)
		{
			CRC = (CRC >> 8) ^ CRCTablesSB8[0][(CRC ^ Ch) & 0xFF];
			CRC = (CRC >> 8) ^ CRCTablesSB8[0][(CRC) & 0xFF];
			CRC = (CRC >> 8) ^ CRCTablesSB8[0][(CRC) & 0xFF];
			CRC = (CRC >> 8) ^ CRCTablesSB8[0][(CRC) & 0xFF];
		}
		return ~CRC;
	}
};