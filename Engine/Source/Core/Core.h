#pragma once
#include <stdio.h>

#ifdef NE_PLATFORM_WINDOWS
	#ifdef NE_BUILD_DLL
		#define NE_API _declspec(dllexport)
	#else
		#define NE_API _declspec(dllimport)
	#endif
#else
	#error We only support windows
#endif

#define FORCEINLINE __forceinline
#define FORCENOINLINE __declspec(noinline)

enum EForceInit
{
	ForceInit,
	ForceInitToZero
};

enum ENoInit { NoInit };

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