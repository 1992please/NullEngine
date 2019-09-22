#pragma once
#include "GL/gl3w.h"
#include "GLFW/glfw3.h"

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