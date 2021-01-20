#pragma once
#include "CoreTypes.h"
#include <ctype.h>
#include <stdarg.h>

#define MAX_SPRINTF 1024

/** Determines case sensitivity options for string comparisons. */
namespace ESearchCase
{
	enum Type
	{
		/** Case sensitive. Upper/lower casing must match for strings to be considered equal. */
		CaseSensitive,

		/** Ignore case. Upper/lower casing does not matter when making a comparison. */
		IgnoreCase,
	};
};

/** Determines search direction for string operations. */
namespace ESearchDir
{
	enum Type
	{
		/** Search from the start, moving forward through the string. */
		FromStart,

		/** Search from the end, moving backward through the string. */
		FromEnd,
	};
}

#define GET_VARARGS_RESULT(msg, msgsize, len, lastarg, fmt, result) \
	{ \
		va_list ap; \
		va_start(ap, lastarg); \
		result = FCString::GetVarArgs(msg, msgsize, fmt, ap); \
		va_end(ap); \
	}

/**
 *	Set of basic string utility functions operating on plain C strings. In addition to the
 *	wrapped C string API,this struct also contains a set of widely used utility functions that
 *  operate on c strings.
 *	There is a specialized implementation for ANSICHAR and WIDECHAR strings provided. To access these
 *	functionality, the convenience typedefs FCString and FCStringAnsi are provided.
 **/

struct FCString
{
	/**
	 * Returns whether this string contains only numeric characters
	 * @param Str - string that will be checked
	 **/
	static bool IsNumeric(const char* Str)
	{
		if (*Str == '-' || *Str == '+')
		{
			Str++;
		}

		bool bHasDot = false;
		while (*Str != '\0')
		{
			if (*Str == '.')
			{
				if (bHasDot)
				{
					return false;
				}
				bHasDot = true;
			}
			else if (!IsDigit(*Str))
			{
				return false;
			}

			++Str;
		}

		return true;
	}


	/**
	 * strcpy wrapper
	 *
	 * @param Dest - destination string to copy to
	 * @param Destcount - size of Dest in characters
	 * @param Src - source string
	 * @return destination string
	 */
	static char* Strcpy(char* Dest, size_t DestCount, const char* Src);

	/**
	 * Copy a string with length checking. Behavior differs from strncpy in that last character is zeroed.
	 *
	 * @param Dest - destination buffer to copy to
	 * @param Src - source buffer to copy from
	 * @param MaxLen - max length of the buffer (including null-terminator)
	 * @return pointer to resulting string buffer
	 */
	static void Strncpy(char* Dest, const char* Src, int32 MaxLen);

	/**
	 * strcpy wrapper
	 * (templated version to automatically handle static destination array case)
	 *
	 * @param Dest - destination string to copy to
	 * @param Src - source string
	 * @return destination string
	 */
	template<size_t DestCount>
	static char* Strcpy(char(&Dest)[DestCount], const char* Src)
	{
		return Strcpy(Dest, DestCount, Src);
	}

	/**
	 * strcat wrapper
	 *
	 * @param Dest - destination string to copy to
	 * @param Destcount - size of Dest in characters
	 * @param Src - source string
	 * @return destination string
	 */
	static char* Strcat(char* Dest, size_t DestCount, const char* Src);


	/**
	 * strcat wrapper
	 * (templated version to automatically handle static destination array case)
	 *
	 * @param Dest - destination string to copy to
	 * @param Src - source string
	 * @return destination string
	 */
	template<size_t DestCount>
	static FORCEINLINE char* Strcat(char(&Dest)[DestCount], const char* Src)
	{
		return Strcat(Dest, DestCount, Src);
	}

	/**
	 * strlen wrapper
	 */
	static int32 NE_API Strlen(const char* String);


	static int32 Strcmp(const char* String1, const char* String2);
	static int32 Strncmp(const char* String1, const char* String2, size_t Count);
	static int32 Stricmp(const char* String1, const char* String2);
	static int32 Strnicmp(const char* String1, const char* String2, size_t Count);

	static const char* Stristr(const char* Str, const char* Find);
	static const char* Strstr(const char* String, const char* Find);

	/**
	* Only converts ASCII characters, same as CRT to[w]upper() with standard C locale
	*/
	static char ToUpper(char Char)
	{
		// make sure first that the character is lower case before changing using the expression (uint32(Char) - 'a' < 26u)
		return (char)(uint8(Char) - ((uint32(Char) - 'a' < 26u) << 5));
	}

	/**
	* Only converts ASCII characters, same as CRT to[w]upper() with standard C locale
	*/
	static char ToLower(char Char)
	{
		return (char)(uint8(Char) + ((uint32(Char) - 'A' < 26u) << 5));
	}

	static int32 Sprintf(char* Dest, const char* Fmt, ...)
	{
		int32	Result = -1;
		GET_VARARGS_RESULT(Dest, MAX_SPRINTF, MAX_SPRINTF - 1, Fmt, Fmt, Result);
		return Result;
	}

	static int32 NE_API GetVarArgs(char* Dest, size_t DestSize, const char*& Fmt, va_list ArgPtr);

	static FORCEINLINE bool IsUpper(char Char) { return ::isupper((unsigned char)Char) != 0; }
	static FORCEINLINE bool IsLower(char Char) { return ::islower((unsigned char)Char) != 0; }
	static FORCEINLINE bool IsAlpha(char Char) { return ::isalpha((unsigned char)Char) != 0; }
	static FORCEINLINE bool IsGraph(char Char) { return ::isgraph((unsigned char)Char) != 0; }
	static FORCEINLINE bool IsPrint(char Char) { return ::isprint((unsigned char)Char) != 0; }
	static FORCEINLINE bool IsPunct(char Char) { return ::ispunct((unsigned char)Char) != 0; }
	static FORCEINLINE bool IsAlnum(char Char) { return ::isalnum((unsigned char)Char) != 0; }
	static FORCEINLINE bool IsDigit(char Char) { return ::isdigit((unsigned char)Char) != 0; }
	static FORCEINLINE bool IsHexDigit(char Char) { return ::isxdigit((unsigned char)Char) != 0; }
	static FORCEINLINE bool IsWhitespace(char Char) { return ::isspace((unsigned char)Char) != 0; }

};