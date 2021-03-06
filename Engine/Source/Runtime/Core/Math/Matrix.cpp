#include "NullPCH.h"
#include "Matrix.h"
#include "Core/Debug/Assert.h"

const FMatrix FMatrix::Identity(FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1), FVector(0, 0, 0));

void FMatrix::SetIdentity()
{
	M[0][0] = 1; M[0][1] = 0;  M[0][2] = 0;  M[0][3] = 0;
	M[1][0] = 0; M[1][1] = 1;  M[1][2] = 0;  M[1][3] = 0;
	M[2][0] = 0; M[2][1] = 0;  M[2][2] = 1;  M[2][3] = 0;
	M[3][0] = 0; M[3][1] = 0;  M[3][2] = 0;  M[3][3] = 1;

}


FMatrix::FMatrix(float value)
{
	M[0][0] = 1.f; M[0][1] = 1.f;  M[0][2] = 1.f;  M[0][3] = 1.f;
	M[1][0] = 1.f; M[1][1] = 1.f;  M[1][2] = 1.f;  M[1][3] = 1.f;
	M[2][0] = 1.f; M[2][1] = 1.f;  M[2][2] = 1.f;  M[2][3] = 1.f;
	M[3][0] = 1.f; M[3][1] = 1.f;  M[3][2] = 1.f;  M[3][3] = 1.f;
}

FString FMatrix::ToString() const
{
	FString Output;

	Output += FString::Printf("[%g %g %g %g] ", M[0][0], M[0][1], M[0][2], M[0][3]);
	Output += FString::Printf("[%g %g %g %g] ", M[1][0], M[1][1], M[1][2], M[1][3]);
	Output += FString::Printf("[%g %g %g %g] ", M[2][0], M[2][1], M[2][2], M[2][3]);
	Output += FString::Printf("[%g %g %g %g] ", M[3][0], M[3][1], M[3][2], M[3][3]);

	return Output;
}
