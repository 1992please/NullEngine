#pragma once
#include "CoreTypes.h"
#include "Less.h"

/**
 * Helper class for dereferencing pointer types in Sort function
 */
template<typename T, class PREDICATE_CLASS>
struct TDereferenceWrapper
{
	const PREDICATE_CLASS& Predicate;

	TDereferenceWrapper(const PREDICATE_CLASS& InPredicate)
		: Predicate(InPredicate) {}

	/** Pass through for non-pointer types */
	FORCEINLINE bool operator()(T& A, T& B) { return Predicate(A, B); }
	FORCEINLINE bool operator()(const T& A, const T& B) const { return Predicate(A, B); }
};
/** Partially specialized version of the above class */
template<typename T, class PREDICATE_CLASS>
struct TDereferenceWrapper<T*, PREDICATE_CLASS>
{
	const PREDICATE_CLASS& Predicate;

	TDereferenceWrapper(const PREDICATE_CLASS& InPredicate)
		: Predicate(InPredicate) {}

	/** Dereference pointers */
	FORCEINLINE bool operator()(T* A, T* B) const
	{
		return Predicate(*A, *B);
	}
};