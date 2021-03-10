#pragma once
#include "Matrix.h"
struct FRotator;

struct alignas(16) FQuat
{
public:
	float X;
	float Y;
	float Z;
	float W;
	static const FQuat Identity;

	/** Default constructor (no initialization). */
	FORCEINLINE FQuat() { }
	explicit FORCEINLINE FQuat(EForceInit);
	FORCEINLINE FQuat(float InX, float InY, float InZ, float InW);
	FORCEINLINE FQuat(const FQuat& Q);
	explicit FQuat(const FMatrix& M);
	explicit FORCEINLINE FQuat(const VectorRegister& V);
	FORCEINLINE FQuat(const FVector& Axis, float AngleRad);
	// R = A * B, applies rotation B then A
	FORCEINLINE FQuat operator*(const FQuat& Q) const;
	FORCEINLINE FQuat operator*=(const FQuat& Q);

	//explicit FQuat(const FRotator& R);
	//FQuat(FVector Axis, float AngleRad);
	FRotator Rotator() const;

	FORCEINLINE FQuat FQuat::Inverse() const
	{
		return FQuat(VectorQuaternionInverse(VectorLoadAligned(this)));
	}

};

FORCEINLINE FQuat::FQuat(EForceInit ZeroOrNot)
	: X(0), Y(0), Z(0), W(ZeroOrNot == ForceInitToZero ? 0.0f : 1.0f)
{ }

FORCEINLINE FQuat::FQuat(float InX, float InY, float InZ, float InW)
	: X(InX)
	, Y(InY)
	, Z(InZ)
	, W(InW)
{}

FORCEINLINE FQuat::FQuat(const FQuat& Q)
	: X(Q.X)
	, Y(Q.Y)
	, Z(Q.Z)
	, W(Q.W)
{ }

inline FQuat::FQuat(const FMatrix& M)
{
	if (M.GetScaledAxis(EAxis::X).IsNearlyZero() || M.GetScaledAxis(EAxis::Y).IsNearlyZero() || M.GetScaledAxis(EAxis::Z).IsNearlyZero())
	{
		*this = FQuat::Identity;
		return;
	}

	//const MeReal *const t = (MeReal *) tm;
	float	s;

	// Check diagonal (trace)
	const float tr = M.M[0][0] + M.M[1][1] + M.M[2][2];

	if (tr > 0.0f)
	{
		float InvS = FMath::InvSqrt(tr + 1.f);
		W = 0.5f * (1.f / InvS);
		s = 0.5f * InvS;

		X = (M.M[1][2] - M.M[2][1]) * s;
		Y = (M.M[2][0] - M.M[0][2]) * s;
		Z = (M.M[0][1] - M.M[1][0]) * s;
	}
	else
	{
		// diagonal is negative
		int32 i = 0;

		if (M.M[1][1] > M.M[0][0])
			i = 1;

		if (M.M[2][2] > M.M[i][i])
			i = 2;

		static const int32 nxt[3] = { 1, 2, 0 };
		const int32 j = nxt[i];
		const int32 k = nxt[j];

		s = M.M[i][i] - M.M[j][j] - M.M[k][k] + 1.0f;

		float InvS = FMath::InvSqrt(s);

		float qt[4];
		qt[i] = 0.5f * (1.f / InvS);

		s = 0.5f * InvS;

		qt[3] = (M.M[j][k] - M.M[k][j]) * s;
		qt[j] = (M.M[i][j] + M.M[j][i]) * s;
		qt[k] = (M.M[i][k] + M.M[k][i]) * s;

		X = qt[0];
		Y = qt[1];
		Z = qt[2];
		W = qt[3];
	}
}

FORCEINLINE FQuat::FQuat(const FVector& Axis, float AngleRad)
{
	const float half_a = 0.5f * AngleRad;
	float s, c;
	FMath::SinCos(&s, &c, half_a);

	X = s * Axis.X;
	Y = s * Axis.Y;
	Z = s * Axis.Z;
	W = c;
}

FORCEINLINE FQuat::FQuat(const VectorRegister& V)
{
	VectorStoreAligned(V, this);
}

FORCEINLINE FQuat FQuat::operator*(const FQuat& Q) const
{
	FQuat Result;
	VectorQuaternionMultiply(&Result, this, &Q);
	return Result;
}


FORCEINLINE FQuat FQuat::operator*=(const FQuat& Q)
{
	VectorQuaternionMultiply(this, this, &Q);
	return *this;
}