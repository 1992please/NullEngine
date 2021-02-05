#pragma once
#include "Core/CoreTypes.h"
#include "Core/Math/SSEMath.h"
#include "NullMath.h"

class ScalarRegister
{
public:
	VectorRegister Value;

	FORCEINLINE ScalarRegister()
	{
	}

	FORCEINLINE ScalarRegister(const ScalarRegister& VectorValue)
	{
		Value = VectorValue.Value;
	}

	FORCEINLINE ScalarRegister(const float& ScalarValue)
	{
		Value = VectorLoadFloat1(&ScalarValue);
	}

	FORCEINLINE ScalarRegister(VectorRegister VectorValue)
	{
		Value = VectorValue;
	}

	FORCEINLINE ScalarRegister operator*(const ScalarRegister& RHS) const
	{
		return ScalarRegister(VectorMultiply(Value, RHS.Value));
	}

	FORCEINLINE ScalarRegister operator+(const ScalarRegister& RHS) const
	{
		return ScalarRegister(VectorAdd(Value, RHS.Value));
	}

	FORCEINLINE ScalarRegister& operator+=(const ScalarRegister& RHS)
	{
		Value = VectorAdd(Value, RHS.Value);
		return *this;
	}

	FORCEINLINE ScalarRegister& operator-=(const ScalarRegister& RHS)
	{
		Value = VectorSubtract(Value, RHS.Value);
		return *this;
	}

	FORCEINLINE ScalarRegister operator-(const ScalarRegister& RHS) const
	{
		return ScalarRegister(VectorSubtract(Value, RHS.Value));
	}

	FORCEINLINE ScalarRegister& operator=(const ScalarRegister& RHS)
	{
		Value = RHS.Value;
		return *this;
	}

	FORCEINLINE ScalarRegister& operator=(const VectorRegister& RHS)
	{
		Value = RHS;
		return *this;
	}

	FORCEINLINE operator VectorRegister() const
	{
		return Value;
	}
};

#define ScalarOne (ScalarRegister)ScalarRegister(VectorOne())
#define ScalarZero (ScalarRegister)ScalarRegister(VectorZero())

template<> FORCEINLINE ScalarRegister FMath::Lerp(const ScalarRegister& A, const ScalarRegister& B, const ScalarRegister& Alpha)
{
	const VectorRegister Delta = VectorSubtract(B.Value, A.Value);
	return ScalarRegister(VectorMultiplyAdd(Alpha.Value, Delta, A.Value));
}