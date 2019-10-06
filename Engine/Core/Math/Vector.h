#pragma once 
#include "NullMath.h"
#include "Rotator.h"

namespace EAxis
{
	enum Type
	{
		None,
		X,
		Y,
		Z,
	};
}

struct FVector
{
public:
	float X;
	float Y;
	float Z;

	static const FVector ZeroVector;
	static const FVector OneVector;
	static const FVector UpVector;
	static const FVector DownVector;
	static const FVector ForwardVector;
	static const FVector BackwardVector;
	static const FVector RightVector;
	static const FVector LeftVector;

	FORCEINLINE FVector();
	explicit FORCEINLINE FVector(float InF);
	FORCEINLINE FVector(float InX, float InY, float InZ);
	//// Cross Product
	FORCEINLINE FVector operator^(const FVector& V) const;
	FORCEINLINE static FVector CrossProduct(const FVector& A, const FVector& B);
	//// Dot Product
	FORCEINLINE float operator|(const FVector& V) const;
	FORCEINLINE static float DotProduct(const FVector& A, const FVector& B);
	FORCEINLINE FVector operator+(const FVector& V) const;
	FORCEINLINE FVector operator-(const FVector& V) const;
	FORCEINLINE FVector operator-(float Bias) const;
	FORCEINLINE FVector operator+(float Bias) const;
	FORCEINLINE FVector operator*(float Scale) const;
	FORCEINLINE FVector operator/(float Scale) const;
	FORCEINLINE FVector operator*(const FVector& V) const;
	FORCEINLINE FVector operator/(const FVector& V) const;
	FORCEINLINE bool operator==(const FVector& V) const;
	FORCEINLINE bool operator!=(const FVector& V) const;
	FORCEINLINE FVector operator-() const;
	FORCEINLINE FVector operator+=(const FVector& V);
	FORCEINLINE FVector operator-=(const FVector& V);
	FORCEINLINE FVector operator*=(float Scale);
	FORCEINLINE FVector operator/=(float V);
	FORCEINLINE FVector operator*=(const FVector& V);
	FORCEINLINE FVector operator/=(const FVector& V);
	inline operator float*() { return &X; }
	inline operator const float*() const { return &X; }
	FORCEINLINE float Size() const;
	FORCEINLINE float SizeSquared() const;
	FORCEINLINE float Size2D() const;
	FORCEINLINE float SizeSquared2D() const;
	FORCEINLINE bool IsNearlyZero(float Tolerance = KINDA_SMALL_NUMBER) const;
	FORCEINLINE bool Normalize(float Tolerance = SMALL_NUMBER);
	FORCEINLINE bool IsNormalized() const;
	FORCEINLINE FVector GetUnsafeNormal() const;
	FORCEINLINE FVector GetSafeNormal(float Tolerance = SMALL_NUMBER) const;
	FORCEINLINE FVector ProjectOnTo(const FVector& A) const;
};


FORCEINLINE FVector::FVector()
{}

FORCEINLINE FVector::FVector(float InF)
	: X(InF), Y(InF), Z(InF)
{}

FORCEINLINE FVector::FVector(float InX, float InY, float InZ)
	: X(InX), Y(InY), Z(InZ)
{}

FORCEINLINE FVector FVector::operator^(const FVector& V) const
{
	return FVector(
		Y * V.Z - Z * V.Y,
		Z * V.X - X * V.Z,
		X * V.Y - Y * V.X
	);
}

FORCEINLINE FVector FVector::CrossProduct(const FVector& A, const FVector& B)
{
	return A ^ B;
}

FORCEINLINE float FVector::operator|(const FVector& V) const
{
	return X * V.X + Y * V.Y + Z * V.Z;
}

FORCEINLINE float FVector::DotProduct(const FVector& A, const FVector& B)
{
	return A | B;
}

FORCEINLINE FVector FVector::operator-(const FVector& V) const
{
	return FVector(X - V.X, Y - V.Y, Z - V.Z);
}

FORCEINLINE FVector FVector::operator-(float Bias) const
{
	return FVector(X - Bias, Y - Bias, Z - Bias);
}

FORCEINLINE FVector FVector::operator+(const FVector& V) const
{
	return FVector(X + V.X, Y + V.Y, Z + V.Z);
}

FORCEINLINE FVector FVector::operator+(float Bias) const
{
	return FVector(X + Bias, Y + Bias, Z + Bias);
}

FORCEINLINE FVector FVector::operator*(float Scale) const
{
	return FVector(X * Scale, Y * Scale, Z * Scale);
}

FORCEINLINE FVector FVector::operator*(const FVector& V) const
{
	return FVector(X * V.X, Y * V.Y, Z * V.Z);
}

FVector FVector::operator/(float Scale) const
{
	return FVector(X / Scale, Y / Scale, Z / Scale);
}

FORCEINLINE FVector FVector::operator/(const FVector& V) const
{
	return FVector(X / V.X, Y / V.Y, Z / V.Z);
}

FORCEINLINE bool FVector::operator==(const FVector& V) const
{
	return X == V.X && Y == V.Y && Z == V.Z;
}

FORCEINLINE bool FVector::operator!=(const FVector& V) const
{
	return X != V.X || Y != V.Y || Z != V.Z;
}

FORCEINLINE FVector FVector::operator-() const
{
	return FVector(-X, -Y, -Z);
}


FORCEINLINE FVector FVector::operator+=(const FVector& V)
{
	X += V.X; Y += V.Y; Z += V.Z;
	return *this;
}

FORCEINLINE FVector FVector::operator-=(const FVector& V)
{
	X -= V.X; Y -= V.Y; Z -= V.Z;
	return *this;
}

FORCEINLINE FVector FVector::operator*=(float Scale)
{
	X *= Scale; Y *= Scale; Z *= Scale;
	return *this;
}

FORCEINLINE FVector FVector::operator/=(float V)
{
	const float RV = 1.f / V;
	X *= RV; Y *= RV; Z *= RV;
	return *this;
}

FORCEINLINE FVector FVector::operator*=(const FVector& V)
{
	X *= V.X; Y *= V.Y; Z *= V.Z;
	return *this;
}

FORCEINLINE FVector FVector::operator/=(const FVector& V)
{
	X /= V.X; Y /= V.Y; Z /= V.Z;
	return *this;
}


FORCEINLINE float FVector::Size() const
{
	return FMath::Sqrt(X * X + Y * Y + Z * Z);
}

FORCEINLINE float FVector::SizeSquared() const
{
	return X * X + Y * Y + Z * Z;
}

FORCEINLINE float FVector::Size2D() const
{
	return FMath::Sqrt(X * X + Y * Y);
}

FORCEINLINE float FVector::SizeSquared2D() const
{
	return X * X + Y * Y;
}

FORCEINLINE bool FVector::IsNearlyZero(float Tolerance /*= KINDA_SMALL_NUMBER*/) const
{
	return
		FMath::Abs(X) <= Tolerance
		&& FMath::Abs(Y) <= Tolerance
		&& FMath::Abs(Z) <= Tolerance;
}

FORCEINLINE bool FVector::Normalize(float Tolerance /*= SMALL_NUMBER*/)
{
	const float SquareSum = X * X + Y * Y + Z * Z;
	if (SquareSum > Tolerance)
	{
		const float Scale = FMath::InvSqrt(SquareSum);
		X *= Scale; Y *= Scale; Z *= Scale;
		return true;
	}
	return false;
}

FORCEINLINE bool FVector::IsNormalized() const
{
	return (FMath::Abs(1.f - SizeSquared()) < THRESH_VECTOR_NORMALIZED);
}

FORCEINLINE FVector FVector::GetUnsafeNormal() const
{
	const float Scale = FMath::InvSqrt(X*X + Y * Y + Z * Z);
	return FVector(X*Scale, Y*Scale, Z*Scale);
}

FORCEINLINE FVector FVector::GetSafeNormal(float Tolerance) const
{
	const float SquareSum = X * X + Y * Y + Z * Z;

	// Not sure if it's safe to add tolerance in there. Might introduce too many errors
	if (SquareSum == 1.f)
	{
		return *this;
	}
	else if (SquareSum < Tolerance)
	{
		return FVector::ZeroVector;
	}
	const float Scale = FMath::InvSqrt(SquareSum);
	return FVector(X*Scale, Y*Scale, Z*Scale);
}

FORCEINLINE FVector FVector::ProjectOnTo(const FVector& A) const
{
	return (A * ((*this | A) / (A | A)));
}
