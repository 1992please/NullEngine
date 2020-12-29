#pragma once
#include "IsArithmetic.h"
#include "IsPointer.h"
#include "AndOrNot.h"
#include "IsEnum.h"

/**
 * TIsZeroConstructType
 */
template<typename T>
struct TIsZeroConstructType
{
	enum { Value = TOr<TIsEnum<T>, TIsArithmetic<T>, TIsPointer<T>>::Value };
};