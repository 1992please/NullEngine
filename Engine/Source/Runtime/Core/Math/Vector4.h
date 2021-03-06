#pragma once
#include "Core/CoreTypes.h"
#include "Core/Math/Vector.h"

struct alignas(16) FVector4
{
public:
	/** The vector's X-component. */
	float X;

	/** The vector's Y-component. */
	float Y;

	/** The vector's Z-component. */
	float Z;

	/** The vector's W-component. */
	float W;
public:
	FORCEINLINE FVector4(const FVector& InVector, float InW = 1.0f);
	explicit FVector4(float InX = 0.0f, float InY = 0.0f, float InZ = 0.0f, float InW = 1.0f);
	explicit FVector4(EForceInit);
	FVector4(const FLinearColor& InColor);


	static const FVector4 ZeroVector;
};


FORCEINLINE FVector4::FVector4(const FVector& InVector, float InW)
	: X(InVector.X)
	, Y(InVector.Y)
	, Z(InVector.Z)
	, W(InW)
{
}

FORCEINLINE FVector4::FVector4(float InX, float InY, float InZ, float InW)
	: X(InX)
	, Y(InY)
	, Z(InZ)
	, W(InW)
{
}


FORCEINLINE FVector4::FVector4(EForceInit)
	: X(0.f)
	, Y(0.f)
	, Z(0.f)
	, W(0.f)
{
}

FORCEINLINE FVector4::FVector4(const FLinearColor& InColor)
	: X(InColor.R)
	, Y(InColor.G)
	, Z(InColor.B)
	, W(InColor.A)
{
}


FVector::FVector(const FVector4& V) : FVector(V.X, V.Y, V.Z) {}