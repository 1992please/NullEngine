#pragma once
#include "Core/CoreTypes.h"
#include "Core/Math/Plane.h"
#include "Core/Math/Vector4.h"
#include "Core/Math/SSEMath.h"
#include "Core/Math/MathUtility.h"
#include "Core/Containers/String.h"

struct alignas(16) FMatrix
{
public:
	float M[4][4];
	static const FMatrix Identity;

	FORCEINLINE FMatrix();

	explicit FORCEINLINE FMatrix(EForceInit)
	{
		FMemory::Memzero(this, sizeof(*this));
	}

	inline void SetIdentity();

	FORCEINLINE FMatrix(const FVector& InX, const FVector& InY, const FVector& InZ, const FVector& InW);
	FORCEINLINE FMatrix(const FPlane& InX, const FPlane& InY, const FPlane& InZ, const FPlane& InW);
	FMatrix(float value);
	FORCEINLINE float* operator[](int32 Other){ return M[Other]; }
	FORCEINLINE void operator*=(const FMatrix& Other);
	FORCEINLINE FMatrix operator*(const FMatrix& Other) const;
	FORCEINLINE FMatrix operator+ (const FMatrix& Other) const;
	FORCEINLINE void operator+=(const FMatrix& Other);
	FORCEINLINE FMatrix operator* (float Other) const;
	FORCEINLINE void operator*=(float Other);
	inline bool operator==(const FMatrix& Other) const;
	inline bool operator!=(const FMatrix& Other) const;
	inline operator float*() { return &M[0][0]; }
	inline operator const float*() const { return &M[0][0]; }

	FORCEINLINE FMatrix GetTransposed() const;

	FORCEINLINE FMatrix InverseFast() const
	{
		FMatrix Result;
		VectorMatrixInverse(&Result, this);
		return Result;
	}

	FORCEINLINE FVector GetScaledAxis(EAxis::Type Axis) const;
	FORCEINLINE FVector4 TransformFVector4(const FVector4& V) const;
	FORCEINLINE FVector4 TransformPosition(const FVector &V) const;
	FORCEINLINE FVector InverseTransformPosition(const FVector &V) const;
	FORCEINLINE FVector4 TransformVector(const FVector& V) const;
	FORCEINLINE FVector InverseTransformVector(const FVector &V) const;

	FORCEINLINE FVector GetOrigin() const;

	inline FMatrix ApplyScale(float Scale) const;

	inline FVector ExtractScaling(float Tolerance = SMALL_NUMBER);

	inline float Determinant() const;

	inline void SetAxis(int32 i, const FVector& Axis);

	FString ToString() const;
};

FORCEINLINE FMatrix::FMatrix()
{
}

FORCEINLINE FMatrix::FMatrix(const FVector& InX, const FVector& InY, const FVector& InZ, const FVector& InW)
{
	M[0][0] = InX.X; M[0][1] = InX.Y;  M[0][2] = InX.Z;  M[0][3] = 0.0f;
	M[1][0] = InY.X; M[1][1] = InY.Y;  M[1][2] = InY.Z;  M[1][3] = 0.0f;
	M[2][0] = InZ.X; M[2][1] = InZ.Y;  M[2][2] = InZ.Z;  M[2][3] = 0.0f;
	M[3][0] = InW.X; M[3][1] = InW.Y;  M[3][2] = InW.Z;  M[3][3] = 1.0f;
}

FORCEINLINE FMatrix::FMatrix(const FPlane& InX, const FPlane& InY, const FPlane& InZ, const FPlane& InW)
{
	M[0][0] = InX.X; M[0][1] = InX.Y;  M[0][2] = InX.Z;  M[0][3] = InX.W;
	M[1][0] = InY.X; M[1][1] = InY.Y;  M[1][2] = InY.Z;  M[1][3] = InY.W;
	M[2][0] = InZ.X; M[2][1] = InZ.Y;  M[2][2] = InZ.Z;  M[2][3] = InZ.W;
	M[3][0] = InW.X; M[3][1] = InW.Y;  M[3][2] = InW.Z;  M[3][3] = InW.W;
}

FORCEINLINE FMatrix FMatrix::operator+(const FMatrix& Other) const
{
	FMatrix ResultMat;

	for (int32 X = 0; X < 4; X++)
	{
		for (int32 Y = 0; Y < 4; Y++)
		{
			ResultMat.M[X][Y] = M[X][Y] + Other.M[X][Y];
		}
	}

	return ResultMat;
}

FORCEINLINE void FMatrix::operator+=(const FMatrix& Other)
{
	*this = *this + Other;
}

FORCEINLINE void FMatrix::operator*=(const FMatrix& Other)
{
	VectorMatrixMultiply(this, this, &Other);
}

FORCEINLINE FMatrix FMatrix::operator*(float Other) const
{
	FMatrix ResultMat;

	for (int32 X = 0; X < 4; X++)
	{
		for (int32 Y = 0; Y < 4; Y++)
		{
			ResultMat.M[X][Y] = M[X][Y] * Other;
		}
	}

	return ResultMat;
}

FORCEINLINE void FMatrix::operator*=(float Other)
{
	*this = *this*Other;
}

FORCEINLINE FMatrix FMatrix::operator*(const FMatrix& Other) const
{
	FMatrix Result;
	VectorMatrixMultiply(&Result, this, &Other);
	return Result;
}

FORCEINLINE FMatrix FMatrix::GetTransposed() const
{
	FMatrix	Result;

	Result.M[0][0] = M[0][0];
	Result.M[0][1] = M[1][0];
	Result.M[0][2] = M[2][0];
	Result.M[0][3] = M[3][0];

	Result.M[1][0] = M[0][1];
	Result.M[1][1] = M[1][1];
	Result.M[1][2] = M[2][1];
	Result.M[1][3] = M[3][1];

	Result.M[2][0] = M[0][2];
	Result.M[2][1] = M[1][2];
	Result.M[2][2] = M[2][2];
	Result.M[2][3] = M[3][2];

	Result.M[3][0] = M[0][3];
	Result.M[3][1] = M[1][3];
	Result.M[3][2] = M[2][3];
	Result.M[3][3] = M[3][3];

	return Result;
}

inline bool FMatrix::operator==(const FMatrix& Other) const
{
	for (int32 X = 0; X < 4; X++)
	{
		for (int32 Y = 0; Y < 4; Y++)
		{
			if (M[X][Y] != Other.M[X][Y])
			{
				return false;
			}
		}
	}

	return true;
}

inline bool FMatrix::operator!=(const FMatrix& Other) const
{
	return !(*this == Other);
}

// Homogeneous transform.

FORCEINLINE FVector4 FMatrix::TransformFVector4(const FVector4 &P) const
{
	FVector4 Result;
	VectorRegister VecP = VectorLoadAligned(&P);
	VectorRegister VecR = VectorTransformVector(VecP, this);
	VectorStoreAligned(VecR, &Result);
	return Result;
}


// Transform position

/** Transform a location - will take into account translation part of the FMatrix. */
FORCEINLINE FVector4 FMatrix::TransformPosition(const FVector &V) const
{
	return TransformFVector4(FVector4(V.X, V.Y, V.Z, 1.0f));
}

/** Inverts the matrix and then transforms V - correctly handles scaling in this matrix. */
FORCEINLINE FVector FMatrix::InverseTransformPosition(const FVector &V) const
{
	FMatrix InvSelf = InverseFast();
	return InvSelf.TransformPosition(V);
}

// Transform vector

/**
 *	Transform a direction vector - will not take into account translation part of the FMatrix.
 *	If you want to transform a surface normal (or plane) and correctly account for non-uniform scaling you should use TransformByUsingAdjointT.
 */
FORCEINLINE FVector4 FMatrix::TransformVector(const FVector& V) const
{
	return TransformFVector4(FVector4(V.X, V.Y, V.Z, 0.0f));
}

/** Faster version of InverseTransformVector that assumes no scaling. WARNING: Will NOT work correctly if there is scaling in the matrix. */
FORCEINLINE FVector FMatrix::InverseTransformVector(const FVector &V) const
{
	FMatrix InvSelf = InverseFast();
	return InvSelf.TransformVector(V);
}

/**
 * Apply Scale to this matrix
 */
inline FMatrix FMatrix::ApplyScale(float Scale) const
{
	FMatrix ScaleMatrix(
		FPlane(Scale, 0.0f, 0.0f, 0.0f),
		FPlane(0.0f, Scale, 0.0f, 0.0f),
		FPlane(0.0f, 0.0f, Scale, 0.0f),
		FPlane(0.0f, 0.0f, 0.0f, 1.0f)
	);
	return ScaleMatrix * (*this);
}

inline FVector FMatrix::ExtractScaling(float Tolerance/*=SMALL_NUMBER*/)
{
	FVector Scale3D(0, 0, 0);

	// For each row, find magnitude, and if its non-zero re-scale so its unit length.
	const float SquareSum0 = (M[0][0] * M[0][0]) + (M[0][1] * M[0][1]) + (M[0][2] * M[0][2]);
	const float SquareSum1 = (M[1][0] * M[1][0]) + (M[1][1] * M[1][1]) + (M[1][2] * M[1][2]);
	const float SquareSum2 = (M[2][0] * M[2][0]) + (M[2][1] * M[2][1]) + (M[2][2] * M[2][2]);

	if (SquareSum0 > Tolerance)
	{
		float Scale0 = FMath::Sqrt(SquareSum0);
		Scale3D[0] = Scale0;
		float InvScale0 = 1.f / Scale0;
		M[0][0] *= InvScale0;
		M[0][1] *= InvScale0;
		M[0][2] *= InvScale0;
	}
	else
	{
		Scale3D[0] = 0;
	}

	if (SquareSum1 > Tolerance)
	{
		float Scale1 = FMath::Sqrt(SquareSum1);
		Scale3D[1] = Scale1;
		float InvScale1 = 1.f / Scale1;
		M[1][0] *= InvScale1;
		M[1][1] *= InvScale1;
		M[1][2] *= InvScale1;
	}
	else
	{
		Scale3D[1] = 0;
	}

	if (SquareSum2 > Tolerance)
	{
		float Scale2 = FMath::Sqrt(SquareSum2);
		Scale3D[2] = Scale2;
		float InvScale2 = 1.f / Scale2;
		M[2][0] *= InvScale2;
		M[2][1] *= InvScale2;
		M[2][2] *= InvScale2;
	}
	else
	{
		Scale3D[2] = 0;
	}

	return Scale3D;
}

FORCEINLINE FVector FMatrix::GetOrigin() const
{
	return FVector(M[3][0], M[3][1], M[3][2]);
}

inline float FMatrix::Determinant() const
{
	return	M[0][0] * (
		M[1][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
		M[2][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) +
		M[3][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2])
		) -
		M[1][0] * (
			M[0][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
			M[2][1] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
			M[3][1] * (M[0][2] * M[2][3] - M[0][3] * M[2][2])
			) +
		M[2][0] * (
			M[0][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) -
			M[1][1] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
			M[3][1] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
			) -
		M[3][0] * (
			M[0][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2]) -
			M[1][1] * (M[0][2] * M[2][3] - M[0][3] * M[2][2]) +
			M[2][1] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
			);
}

inline void FMatrix::SetAxis(int32 i, const FVector& Axis)
{
	NE_ASSERT(i >= 0 && i <= 2);
	M[i][0] = Axis.X;
	M[i][1] = Axis.Y;
	M[i][2] = Axis.Z;
}