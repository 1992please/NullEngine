#pragma once
#include "NullMemory.h"
#include "Plane.h"
#include "Vector4.h"

struct alignas(16) FMatrix
{
public:
	float M[4][4];
	static const FMatrix Identity;

	// Constructors.
	FORCEINLINE FMatrix();

	/**
 * Constructor.
 *
 * @param EForceInit Force Init Enum.
 */
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

	inline FMatrix InverseFast() const
	{
		FMatrix Result;
		VectorMatrixInverse(&Result, this);
		return Result;
	}

	inline FVector GetScaledAxis(EAxis::Type Axis) const;

	// Homogeneous transform.
	FORCEINLINE FVector4 TransformFVector4(const FVector4& V) const;

	/** Transform a location - will take into account translation part of the FMatrix. */
	FORCEINLINE FVector4 TransformPosition(const FVector &V) const;

	/** Inverts the matrix and then transforms V - correctly handles scaling in this matrix. */
	FORCEINLINE FVector InverseTransformPosition(const FVector &V) const;

	/**
	 *	Transform a direction vector - will not take into account translation part of the FMatrix.
	 *	If you want to transform a surface normal (or plane) and correctly account for non-uniform scaling you should use TransformByUsingAdjointT.
	 */
	FORCEINLINE FVector4 TransformVector(const FVector& V) const;

	/**
	 *	Transform a direction vector by the inverse of this matrix - will not take into account translation part.
	 *	If you want to transform a surface normal (or plane) and correctly account for non-uniform scaling you should use TransformByUsingAdjointT with adjoint of matrix inverse.
	 */
	FORCEINLINE FVector InverseTransformVector(const FVector &V) const;

	static FMatrix Translate(float X, float Y, float Z)
	{
		FMatrix Result;
		Result[0][0] = 1; Result[0][1] = 0;  Result[0][2] = 0;  Result[0][3] = 0;
		Result[1][0] = 0; Result[1][1] = 1;  Result[1][2] = 0;  Result[1][3] = 0;
		Result[2][0] = 0; Result[2][1] = 0;  Result[2][2] = 1;  Result[2][3] = 0;
		Result[3][0] = X; Result[3][1] = Y;  Result[3][2] = Z;  Result[3][3] = 1;
		return Result;

	}

	static FMatrix Rotate(FMatrix const& InMat, float angle, FVector InVec)
	{
		float const a = angle;
		float const c = FMath::Cos(a);
		float const s = FMath::Sin(a);

		FVector axis(InVec.GetSafeNormal());
		FVector temp(axis * (1.0f - c));

		FMatrix RotateMat(ForceInit);
		RotateMat[0][0] = c + temp[0] * axis[0];
		RotateMat[0][1] = temp[0] * axis[1] + s * axis[2];
		RotateMat[0][2] = temp[0] * axis[2] - s * axis[1];

		RotateMat[1][0] = temp[1] * axis[0] - s * axis[2];
		RotateMat[1][1] = c + temp[1] * axis[1];
		RotateMat[1][2] = temp[1] * axis[2] + s * axis[0];

		RotateMat[2][0] = temp[2] * axis[0] + s * axis[1];
		RotateMat[2][1] = temp[2] * axis[1] - s * axis[0];
		RotateMat[2][2] = c + temp[2] * axis[2];
		RotateMat[3][3] = 1.0f;
		FMatrix Result = RotateMat * InMat;

		return Result;
	}

	static FMatrix Rotate(float angle, FVector InVec)
	{
		float const a = angle;
		float const c = FMath::Cos(a);
		float const s = FMath::Sin(a);

		FVector axis(InVec.GetSafeNormal());
		FVector temp(axis * (1.0f - c));

		FMatrix RotateMat;
		RotateMat[0][0] = c + temp[0] * axis[0];
		RotateMat[0][1] = temp[0] * axis[1] + s * axis[2];
		RotateMat[0][2] = temp[0] * axis[2] - s * axis[1];

		RotateMat[1][0] = temp[1] * axis[0] - s * axis[2];
		RotateMat[1][1] = c + temp[1] * axis[1];
		RotateMat[1][2] = temp[1] * axis[2] + s * axis[0];

		RotateMat[2][0] = temp[2] * axis[0] + s * axis[1];
		RotateMat[2][1] = temp[2] * axis[1] - s * axis[0];
		RotateMat[2][2] = c + temp[2] * axis[2];

		return RotateMat;
	}

	/** Apply Scale to this matrix **/
	inline FMatrix ApplyScale(float Scale) const;

};

struct FLookAtMatrix : FMatrix
{
	/**
	 * Creates a view matrix given an eye position, a position to look at, and an up vector.
	 * This does the same thing as D3DXMatrixLookAtLH.
	 */
	FLookAtMatrix(const FVector& EyePosition, const FVector& LookAtPosition, const FVector& UpVector);
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


FORCEINLINE FLookAtMatrix::FLookAtMatrix(const FVector& EyePosition, const FVector& LookAtPosition, const FVector& UpVector)
{
	const FVector ZAxis = (LookAtPosition - EyePosition).GetSafeNormal();
	const FVector XAxis = (UpVector ^ ZAxis).GetSafeNormal();
	const FVector YAxis = ZAxis ^ XAxis;

	for (uint32 RowIndex = 0; RowIndex < 3; RowIndex++)
	{
		M[RowIndex][0] = (&XAxis.X)[RowIndex];
		M[RowIndex][1] = (&YAxis.X)[RowIndex];
		M[RowIndex][2] = (&ZAxis.X)[RowIndex];
		M[RowIndex][3] = 0.0f;
	}
	M[3][0] = -EyePosition | XAxis;
	M[3][1] = -EyePosition | YAxis;
	M[3][2] = -EyePosition | ZAxis;
	M[3][3] = 1.0f;
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
