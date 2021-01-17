#pragma once
#include "Windows/WindowSystemIncludes.h"

typedef signed char        int8;
typedef short              int16;
typedef int                int32;
typedef long long          int64;
typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
typedef unsigned long long uint64;

#define RESTRICT __restrict

#define FORCEINLINE     __forceinline
#define FORCENOINLINE   __declspec(noinline)

#define CONSTEXPR constexpr

#ifdef NE_BUILD_DLL
	#define NE_API _declspec(dllexport)
#else
	#define NE_API _declspec(dllimport)
#endif

enum { INDEX_NONE = -1 };

enum EForceInit
{
	ForceInit,
	ForceInitToZero
};

enum ENoInit { NoInit };