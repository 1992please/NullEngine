#pragma once
#include "CoreTypes.h"
#include "IsArithmetic.h"
#include "IsPointer.h"
#include "AndOrNot.h"
#include "IsEnum.h"
#include "AreTypesEqual.h"
#include "IsTriviallyCopyConstructible.h"
#include "IsPODType.h"
/**
 * TIsZeroConstructType
 */
template<typename T>
struct TIsZeroConstructType
{
	enum { Value = TOr<TIsEnum<T>, TIsArithmetic<T>, TIsPointer<T>>::Value };
};

/**
 * TIsReferenceType
 */
template<typename T> struct TIsReferenceType { enum { Value = false }; };
template<typename T> struct TIsReferenceType<T&> { enum { Value = true }; };
template<typename T> struct TIsReferenceType<T&&> { enum { Value = true }; };

/**
 * TIsLValueReferenceType
 */
template<typename T> struct TIsLValueReferenceType { enum { Value = false }; };
template<typename T> struct TIsLValueReferenceType<T&> { enum { Value = true }; };

/**
 * TIsRValueReferenceType
 */
template<typename T> struct TIsRValueReferenceType { enum { Value = false }; };
template<typename T> struct TIsRValueReferenceType<T&&> { enum { Value = true }; };



/**
 * Tests if a type T is bitwise-constructible from a given argument type U.  That is, whether or not
 * the U can be memcpy'd in order to produce an instance of T, rather than having to go
 * via a constructor.
 *
 * Examples:
 * TIsBitwiseConstructible<PODType,    PODType   >::Value == true  // PODs can be trivially copied
 * TIsBitwiseConstructible<const int*, int*      >::Value == true  // a non-const Derived pointer is trivially copyable as a const Base pointer
 * TIsBitwiseConstructible<int*,       const int*>::Value == false // not legal the other way because it would be a const-correctness violation
 * TIsBitwiseConstructible<int32,      uint32    >::Value == true  // signed integers can be memcpy'd as unsigned integers
 * TIsBitwiseConstructible<uint32,     int32     >::Value == true  // and vice versa
 */

template <typename T, typename Arg>
struct TIsBitwiseConstructible
{
	static_assert(
		!TIsReferenceType<T  >::Value &&
		!TIsReferenceType<Arg>::Value,
		"TIsBitwiseConstructible is not designed to accept reference types");

	static_assert(
		TAreTypesEqual<T, typename TRemoveCV<T  >::Type>::Value &&
		TAreTypesEqual<Arg, typename TRemoveCV<Arg>::Type>::Value,
		"TIsBitwiseConstructible is not designed to accept qualified types");

	// Assume no bitwise construction in general
	enum { Value = false };
};

template <typename T>
struct TIsBitwiseConstructible<T, T>
{
	// Ts can always be bitwise constructed from itself if it is trivially copyable.
	enum { Value = TIsTriviallyCopyConstructible<T>::Value };
};

template <typename T, typename U>
struct TIsBitwiseConstructible<const T, U> : TIsBitwiseConstructible<T, U>
{
	// Constructing a const T is the same as constructing a T
};

// Const pointers can be bitwise constructed from non-const pointers.
// This is not true for pointer conversions in general, e.g. where an offset may need to be applied in the case
// of multiple inheritance, but there is no way of detecting that at compile-time.
template <typename T>
struct TIsBitwiseConstructible<const T*, T*>
{
	// Constructing a const T is the same as constructing a T
	enum { Value = true };
};

// Unsigned types can be bitwise converted to their signed equivalents, and vice versa.
// (assuming two's-complement, which we are)
template <> struct TIsBitwiseConstructible<uint8, int8> { enum { Value = true }; };
template <> struct TIsBitwiseConstructible< int8, uint8> { enum { Value = true }; };
template <> struct TIsBitwiseConstructible<uint16, int16> { enum { Value = true }; };
template <> struct TIsBitwiseConstructible< int16, uint16> { enum { Value = true }; };
template <> struct TIsBitwiseConstructible<uint32, int32> { enum { Value = true }; };
template <> struct TIsBitwiseConstructible< int32, uint32> { enum { Value = true }; };
template <> struct TIsBitwiseConstructible<uint64, int64> { enum { Value = true }; };
template <> struct TIsBitwiseConstructible< int64, uint64> { enum { Value = true }; };

/*-----------------------------------------------------------------------------
	Call traits - Modeled somewhat after boost's interfaces.
-----------------------------------------------------------------------------*/

/**
 * Call traits helpers
 */
template <typename T, bool TypeIsSmall>
struct TCallTraitsParamTypeHelper
{
	typedef const T& ParamType;
	typedef const T& ConstParamType;
};
template <typename T>
struct TCallTraitsParamTypeHelper<T, true>
{
	typedef const T ParamType;
	typedef const T ConstParamType;
};
template <typename T>
struct TCallTraitsParamTypeHelper<T*, true>
{
	typedef T* ParamType;
	typedef const T* ConstParamType;
};


/*-----------------------------------------------------------------------------
 * TCallTraits
 *
 * Same call traits as boost, though not with as complete a solution.
 *
 * The main member to note is ParamType, which specifies the optimal
 * form to pass the type as a parameter to a function.
 *
 * Has a small-value optimization when a type is a POD type and as small as a pointer.
-----------------------------------------------------------------------------*/

/**
 * base class for call traits. Used to more easily refine portions when specializing
 */
template <typename T>
struct TCallTraitsBase
{
private:
	enum { PassByValue = TOr<TAndValue<(sizeof(T) <= sizeof(void*)), TIsPODType<T>>, TIsArithmetic<T>>::Value };

public:
	typedef T ValueType;
	typedef T& Reference;
	typedef const T& ConstReference;
	typedef typename TCallTraitsParamTypeHelper<T, PassByValue>::ParamType ParamType;
	typedef typename TCallTraitsParamTypeHelper<T, PassByValue>::ConstParamType ConstPointerType;
};

/**
 * TCallTraits
 */
template <typename T>
struct TCallTraits : public TCallTraitsBase<T> {};

// Fix reference-to-reference problems.
template <typename T>
struct TCallTraits<T&>
{
	typedef T& ValueType;
	typedef T& Reference;
	typedef const T& ConstReference;
	typedef T& ParamType;
	typedef T& ConstPointerType;
};

// Array types
template <typename T, size_t N>
struct TCallTraits<T[N]>
{
private:
	typedef T ArrayType[N];
public:
	typedef const T* ValueType;
	typedef ArrayType& Reference;
	typedef const ArrayType& ConstReference;
	typedef const T* const ParamType;
	typedef const T* const ConstPointerType;
};

// const array types
template <typename T, size_t N>
struct TCallTraits<const T[N]>
{
private:
	typedef const T ArrayType[N];
public:
	typedef const T* ValueType;
	typedef ArrayType& Reference;
	typedef const ArrayType& ConstReference;
	typedef const T* const ParamType;
	typedef const T* const ConstPointerType;
};


/*-----------------------------------------------------------------------------
	Traits for our particular container classes
-----------------------------------------------------------------------------*/

/**
 * Helper for array traits. Provides a common base to more easily refine a portion of the traits
 * when specializing. Mainly used by MemoryOps.h which is used by the contiguous storage containers like TArray.
 */
template<typename T>
struct TTypeTraitsBase
{
	typedef typename TCallTraits<T>::ParamType ConstInitType;
	typedef typename TCallTraits<T>::ConstPointerType ConstPointerType;

	// There's no good way of detecting this so we'll just assume it to be true for certain known types and expect
	// users to customize it for their custom types.
	enum { IsBytewiseComparable = TOr<TIsEnum<T>, TIsArithmetic<T>, TIsPointer<T>>::Value };
};

/**
 * Traits for types.
 */
template<typename T> struct TTypeTraits : public TTypeTraitsBase<T> {};