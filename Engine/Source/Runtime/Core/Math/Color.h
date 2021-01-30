#pragma once
#include "Core/CoreTypes.h"
#include "Core/Math/MathUtility.h"
#include "Core/Containers/String.h"

struct FLinearColor
{
	float R;
	float G;
	float B;
	float A;

	FORCEINLINE FLinearColor() {}
	FORCEINLINE explicit FLinearColor(EForceInit)
		: R(0), G(0), B(0), A(0)
	{}
	constexpr FORCEINLINE FLinearColor(float InR, float InG, float InB, float InA = 1.0f) : R(InR), G(InG), B(InB), A(InA) {}


	FLinearColor(const struct FVector& Vector);

	explicit FLinearColor(const struct FVector4& Vector);

	/** Comparison operators */
	FORCEINLINE bool operator==(const FLinearColor& ColorB) const
	{
		return this->R == ColorB.R && this->G == ColorB.G && this->B == ColorB.B && this->A == ColorB.A;
	}
	FORCEINLINE bool operator!=(const FLinearColor& Other) const
	{
		return this->R != Other.R || this->G != Other.G || this->B != Other.B || this->A != Other.A;
	}

	// Error-tolerant comparison.
	FORCEINLINE bool Equals(const FLinearColor& ColorB, float Tolerance = KINDA_SMALL_NUMBER) const
	{
		return FMath::Abs(this->R - ColorB.R) < Tolerance && FMath::Abs(this->G - ColorB.G) < Tolerance && FMath::Abs(this->B - ColorB.B) < Tolerance && FMath::Abs(this->A - ColorB.A) < Tolerance;
	}

	FString ToString() const
	{
		return FString::Printf("(R=%f,G=%f,B=%f,A=%f)", R, G, B, A);
	}


	// Common colors.	
	static const FLinearColor White;
	static const FLinearColor Gray;
	static const FLinearColor Black;
	static const FLinearColor Transparent;
	static const FLinearColor Red;
	static const FLinearColor Green;
	static const FLinearColor Blue;
	static const FLinearColor Yellow;
};