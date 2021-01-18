#pragma once
#include "CoreTypes.h"
#include "Memory/NullMemory.h"
#include "RemoveReference.h"
#include "IsEnum.h"
#include "IsArithmetic.h"
#include "IsPointer.h"
#include "TypeTraits.h"
#include "EnableIf.h"
#include "AreTypesEqual.h"
#include "AndOrNot.h"
#include "TypeCompatibleBytes.h"
#include "MemoryOps.h"

/**
 * MoveTemp will cast a reference to an rvalue reference.
 * This is UE's equivalent of std::move except that it will not compile when passed an rvalue or
 * const object, because we would prefer to be informed when MoveTemp will have no effect.
 */
template <typename T>
FORCEINLINE typename TRemoveReference<T>::Type&& MoveTemp(T&& Obj)
{
	typedef typename TRemoveReference<T>::Type CastType;

	// Validate that we're not being passed an rvalue or a const object - the former is redundant, the latter is almost certainly a mistake
	static_assert(TIsLValueReferenceType<T>::Value, "MoveTemp called on an rvalue");
	static_assert(!TAreTypesEqual<CastType&, const CastType&>::Value, "MoveTemp called on a const object");

	return (CastType&&)Obj;
}

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


/** This is used to provide type specific behavior for a move which will destroy B. */
/** Should be in UnrealTemplate but isn't for Clang build reasons - will move later */
template<typename T>
FORCEINLINE void MoveByRelocate(T& A, T& B)
{
	// Destruct the previous value of A.
	A.~T();

	// Relocate B into the 'hole' left by the destruction of A, leaving a hole in B instead.
	RelocateConstructItems<T>(&A, &B, 1);
}


/**
 * A traits class which specifies whether a Swap of a given type should swap the bits or use a traditional value-based swap.
 */
template <typename T>
struct TUseBitwiseSwap
{
	// We don't use bitwise swapping for 'register' types because this will force them into memory and be slower.
	enum { Value = !TOr<TIsEnum<T>, TIsPointer<T>, TIsArithmetic<T>>::Value };
};

/**
 * Swap two values.  Assumes the types are trivially relocatable.
 */
template <typename T>
inline typename TEnableIf<TUseBitwiseSwap<T>::Value>::Type Swap(T& A, T& B)
{
	if (&A != &B)
	{
		TTypeCompatibleBytes<T> Temp;
		FMemory::Memcpy(&Temp, &A, sizeof(T));
		FMemory::Memcpy(&A, &B, sizeof(T));
		FMemory::Memcpy(&B, &Temp, sizeof(T));
	}
}

template <typename T>
inline typename TEnableIf<!TUseBitwiseSwap<T>::Value>::Type Swap(T& A, T& B)
{
	T Temp = MoveTemp(A);
	A = MoveTemp(B);
	B = MoveTemp(Temp);
}

/**
 * Reverses the order of the bits of a value.
 * This is an TEnableIf'd template to ensure that no undesirable conversions occur.  Overloads for other types can be added in the same way.
 *
 * @param Bits - The value to bit-swap.
 * @return The bit-swapped value.
 */
template <typename T>
FORCEINLINE typename TEnableIf<TAreTypesEqual<T, uint32>::Value, T>::Type ReverseBits(T Bits)
{
	Bits = (Bits << 16) | (Bits >> 16);
	Bits = ((Bits & 0x00ff00ff) << 8) | ((Bits & 0xff00ff00) >> 8);
	Bits = ((Bits & 0x0f0f0f0f) << 4) | ((Bits & 0xf0f0f0f0) >> 4);
	Bits = ((Bits & 0x33333333) << 2) | ((Bits & 0xcccccccc) >> 2);
	Bits = ((Bits & 0x55555555) << 1) | ((Bits & 0xaaaaaaaa) >> 1);
	return Bits;
}

/**
 * Equivalent to std::declval.
 *
 * Note that this function is unimplemented, and is only intended to be used in unevaluated contexts, like sizeof and trait expressions.
 */
template <typename T>
T&& DeclVal();

/**
 * TRValueToLValueReference converts any rvalue reference type into the equivalent lvalue reference, otherwise returns the same type.
 */
template <typename T> struct TRValueToLValueReference { typedef T  Type; };
template <typename T> struct TRValueToLValueReference<T&&> { typedef T& Type; };


/**
 * This exists to avoid a Visual Studio bug where using a cast to forward an rvalue reference array argument
 * to a pointer parameter will cause bad code generation.  Wrapping the cast in a function causes the correct
 * code to be generated.
 */
template <typename T, typename ArgType>
FORCEINLINE T StaticCast(ArgType&& Arg)
{
	return static_cast<T>(Arg);
}

// Number of elements in an array.
template <typename T, uint32 N>
char(&UE4ArrayCountHelper(const T(&)[N]))[N + 1];

#define UE_ARRAY_COUNT( array ) (sizeof(UE4ArrayCountHelper(array)) - 1)