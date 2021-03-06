#include "NullPCH.h"
#include "CString.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static constexpr uint8 LowerAscii[128] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',
	'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',  0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F
};

FORCEINLINE bool BothAscii(char C1, char C2)
{
	return (((uint32)C1 | (uint32)C2) & 0xffffff80) == 0;
}

char* FCString::Strcpy(char* Dest, size_t DestCount, const char* Src)
{
	strcpy_s(Dest, DestCount, Src);
	return Dest;
}

char* FCString::Strcat(char* Dest, size_t DestCount, const char* Src)
{
	strcat_s(Dest, DestCount, Src);
	return Dest;
}

int32 FCString::Atoi(const char* String)
{
	return atoi(String);
}

float FCString::Atof(const char* String)
{
	return (float)atof(String);
}

int32 FCString::Strlen(const char* String)
{
	return (int32)strlen(String);
}

int32 FCString::Strcmp(const char* String1, const char* String2)
{
	return strcmp(String1, String2);
}

int32 FCString::Strncmp(const char* String1, const char* String2, size_t Count)
{
	return strncmp(String1, String2, Count);
}

int32 FCString::Stricmp(const char* String1, const char* String2)
{
	while (true)
	{
		char C1 = *String1++;
		char C2 = *String2++;

		// Quickly move on if characters are identical but
		// return equals if we found two null terminators
		if (C1 == C2)
		{
			if (C1)
			{
				continue;
			}

			return 0;
		}
		else if (BothAscii(C1, C2))
		{
			if (int32 Diff = LowerAscii[uint8(C1)] - LowerAscii[uint8(C2)])
			{
				return Diff;
			}
		}
		else
		{
			return uint8(C1) - uint8(C2);
		}
	}
}

int32 FCString::Strnicmp(const char* String1, const char* String2, size_t Count)
{
	for (; Count > 0; --Count)
	{
		char C1 = *String1++;
		char C2 = *String2++;

		// Quickly move on if characters are identical but
		// return equals if we found two null terminators
		if (C1 == C2)
		{
			if (C1)
			{
				continue;
			}

			return 0;
		}
		else if (BothAscii(C1, C2))
		{
			if (int32 Diff = LowerAscii[uint8(C1)] - LowerAscii[uint8(C2)])
			{
				return Diff;
			}
		}
		else
		{
			return uint8(C1) - uint8(C2);
		}
	}

	return 0;
}

const char* FCString::Stristr(const char* Str, const char* Find)
{
	// both strings must be valid
	if (Find == NULL || Str == NULL)
	{
		return NULL;
	}

	// get upper-case first letter of the find string (to reduce the number of full strnicmps)
	char FindInitial = ToUpper(*Find);
	// get length of find string, and increment past first letter
	int32   Length = Strlen(Find++) - 1;
	// get the first letter of the search string, and increment past it
	char StrChar = *Str++;
	// while we aren't at end of string...
	while (StrChar)
	{
		// make sure it's upper-case
		StrChar = ToUpper(StrChar);
		// if it matches the first letter of the find string, do a case-insensitive string compare for the length of the find string
		if (StrChar == FindInitial && !Strnicmp(Str, Find, Length))
		{
			// if we found the string, then return a pointer to the beginning of it in the search string
			return Str - 1;
		}
		// go to next letter
		StrChar = *Str++;
	}

	// if nothing was found, return NULL
	return NULL;
}

const char* FCString::Strstr(const char* String, const char* Find)
{
	return strstr(String, Find);
}

const char* FCString::StrChar(const char* String, const char Find)
{
	return strchr(String, Find);
}

int32 FCString::GetVarArgs(char* Dest, size_t DestSize, const char*& Fmt, va_list ArgPtr)
{
	int32 Result = vsnprintf(Dest, DestSize, Fmt, ArgPtr);
	va_end(ArgPtr);
	return (Result != -1 && Result < (int32)DestSize) ? Result : -1;
}

void FCString::Strncpy(char* Dest, const char* Src, int32 MaxLen)
{
	strncpy_s(Dest, MaxLen, Src, MaxLen);
	Dest[MaxLen - 1] = 0;
}