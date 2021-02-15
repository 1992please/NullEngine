#pragma once
#include "Core/CoreTypes.h"
#include "Vector.h"

struct FVector2
{
	/** Vector's X component. */
	float X;

	/** Vector's Y component. */
	float Y;
public:
	/** Default constructor (no initialization). */
	FORCEINLINE FVector2() { }
	FORCEINLINE FVector2(float InX, float InY);
	FORCEINLINE FVector2(float Value);
	FORCEINLINE FVector2(const FVector& InVec);
	explicit FORCEINLINE FVector2(EForceInit);
	explicit FORCEINLINE FVector2(ENoInit) { }
	FORCEINLINE bool IsNearlyZero(float Tolerance = KINDA_SMALL_NUMBER) const;

	FORCEINLINE FVector2 operator+=(const FVector2& V);
	FORCEINLINE bool operator!=(const FVector2& V) const;
	FORCEINLINE bool operator==(const FVector2& V) const;

	FORCEINLINE FString ToString() const
	{
		return FString::Printf("X=%3.3f Y=%3.3f", X, Y);
	}
	static const FVector2 ZeroVector;
};

FORCEINLINE FVector2::FVector2(float InX, float InY)
	: X(InX), Y(InY)
{ }

FORCEINLINE FVector2::FVector2(EForceInit)
	: X(0), Y(0)
{
}

FORCEINLINE FVector2::FVector2(float Value)
	: X(Value), Y(Value)
{

}

FORCEINLINE FVector2::FVector2(const FVector& InVec)
	: X(InVec.X), Y(InVec.Y)
{

}

FORCEINLINE FVector::FVector(const FVector2& V)
	: X(V.X), Y(V.Y), Z(0.0f)
{

}

FORCEINLINE bool FVector2::IsNearlyZero(float Tolerance /*= KINDA_SMALL_NUMBER*/) const
{
	return
		FMath::Abs(X) <= Tolerance
		&& FMath::Abs(Y) <= Tolerance;
}

FORCEINLINE FVector2 FVector2::operator+=(const FVector2& V)
{
	X += V.X; Y += V.Y;
	return *this;
}

FORCEINLINE bool FVector2::operator!=(const FVector2& V) const
{
	return X != V.X || Y != V.Y;
}

FORCEINLINE bool FVector2::operator==(const FVector2& V) const
{
	return X == V.X && Y == V.Y;
}