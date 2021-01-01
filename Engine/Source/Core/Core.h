#pragma once
#include <stdio.h>

typedef signed char        int8;
typedef short              int16;
typedef int                int32;
typedef long long          int64;
typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
typedef unsigned long long uint64;

#define FORCEINLINE     __forceinline
#define FORCENOINLINE   __declspec(noinline)

#ifdef NE_PLATFORM_WINDOWS
	#ifdef NE_BUILD_DLL
		#define NE_API _declspec(dllexport)
	#else
		#define NE_API _declspec(dllimport)
	#endif
#else
	#error We only support windows
#endif

enum { INDEX_NONE = -1 };

enum EForceInit
{
	ForceInit,
	ForceInitToZero
};

enum ENoInit { NoInit };


#if _DEBUG
static void reportAssertionFailure(const char* expr, const char* file, int line)
{
	printf("%s\n %s\n %d\n", expr, file, line);
}

// check the expression and fail if it is false
#define ASSERT(expr) \
	if (expr) { } \
	else \
	{ \
	reportAssertionFailure(#expr, \
	__FILE__, __LINE__); \
	__debugbreak(); \
	}
#else
#define ASSERT(expr) // evaluates to nothing
#endif