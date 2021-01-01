#pragma once
#include "Core.h"
#include "NullMemory.h"
#include "RemoveReference.h"
#include "IsEnum.h"
#include "IsArithmetic.h"
#include "IsPointer.h"
#include "TypeTraits.h"
#include "EnableIf.h"
#include "AreTypesEqual.h"
#include "AndOrNot.h"
#include "TypeCompatibleBytes.h"

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