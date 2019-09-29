#pragma once
#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include "stdio.h"

#define FORCEINLINE __forceinline

enum EForceInit
{
	ForceInit,
	ForceInitToZero
};

typedef signed char        int8;
typedef short              int16;
typedef int                int32;
typedef long long          int64;
typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
typedef unsigned long long uint64;

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