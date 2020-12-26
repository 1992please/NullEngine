#pragma once
#include <stdio.h>

#define FORCEINLINE __forceinline
#define FORCENOINLINE __declspec(noinline)

enum EForceInit
{
	ForceInit,
	ForceInitToZero
};

enum ENoInit { NoInit };

typedef signed char        int8;
typedef short              int16;
typedef int                int32;
typedef long long          int64;
typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
typedef unsigned long long uint64;

/**
 * TRemoveReference<type> will remove any references from a type.
 */
template <typename T> struct TRemoveReference { typedef T Type; };
template <typename T> struct TRemoveReference<T& > { typedef T Type; };
template <typename T> struct TRemoveReference<T&&> { typedef T Type; };

/**
 * MoveTemp will cast a reference to an rvalue reference.
 * This is UE's equivalent of std::move.  It doesn't static assert like MoveTemp, because it is useful in
 * templates or macros where it's not obvious what the argument is, but you want to take advantage of move semantics
 * where you can but not stop compilation.
 */
template <typename T>
FORCEINLINE typename TRemoveReference<T>::Type&& MoveTempIfPossible(T&& Obj)
{
	return (typename TRemoveReference<T>::Type&&)Obj;
}

 /**
  * Forward will cast a reference to an rvalue reference.
  * This is UE's equivalent of std::forward.
  */
template <typename T>
FORCEINLINE T&& Forward(typename TRemoveReference<T>::Type& Obj)
{
	return (T&&)Obj;
}

template <typename T>
FORCEINLINE T&& Forward(typename TRemoveReference<T>::Type&& Obj)
{
	return (T&&)Obj;
}

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