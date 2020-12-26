#pragma once
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
	explicit FORCEINLINE FVector2(EForceInit);
	explicit FORCEINLINE FVector2(ENoInit) { }
};

FORCEINLINE FVector2::FVector2(float InX, float InY)
	: X(InX), Y(InY)
{ }

FORCEINLINE FVector2::FVector2(EForceInit)
	: X(0), Y(0)
{
}