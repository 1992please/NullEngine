#pragma once

typedef signed char        int8;
typedef short              int16;
typedef int                int32;
typedef long long          int64;
typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
typedef unsigned long long uint64;

#define RESTRICT __restrict

// Function type macros.
#define FORCEINLINE     __forceinline
#define FORCENOINLINE   __declspec(noinline)

#define CONSTEXPR constexpr

#if NE_DEBUG
	#define NE_DEBUGBREAK()  __debugbreak()
#else
	#define NE_DEBUGBREAK
#endif
enum { INDEX_NONE = -1 };

enum EForceInit
{
	ForceInit,
	ForceInitToZero
};

enum ENoInit { NoInit };


#ifndef NE_DEBUG
	#define NE_DEBUG 0
#endif

#ifndef NE_RELEASE
	#define NE_RELEASE 0
#endif

#ifndef NE_SHIPPING
	#define NE_SHIPPING 0
#endif

#ifndef NE_WINDOWS
	#define NE_WINDOWS 0
#endif

#if NE_WINDOWS
	#pragma warning(disable: 4291) //4307 using new without delete
#endif