#pragma once
#include "Core/CoreTypes.h"
#include "Core/Math/Matrix.h"

struct FLookFromMatrix : FMatrix
{
	FORCEINLINE FLookFromMatrix(const FVector& EyePosition, const FVector& LookDirection, const FVector& UpVector)
    {
	const FVector ZAxis = LookDirection.GetSafeNormal();
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
};


struct FLookAtMatrix : FLookFromMatrix
{
    FORCEINLINE FLookAtMatrix(const FVector& EyePosition, const FVector& LookAtPosition, const FVector& UpVector) 
        : FLookFromMatrix(EyePosition, LookAtPosition - EyePosition, UpVector)
    {}
};