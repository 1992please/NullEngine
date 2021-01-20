#include "Matrix.h"
#include "Core/Logging/Logger.h"

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

inline FVector FMatrix::GetScaledAxis(EAxis::Type InAxis) const
{
	switch (InAxis)
	{
		case EAxis::X:
			return FVector(M[0][0], M[0][1], M[0][2]);

		case EAxis::Y:
			return FVector(M[1][0], M[1][1], M[1][2]);

		case EAxis::Z:
			return FVector(M[2][0], M[2][1], M[2][2]);

		default:
			NE_ASSERT(0)
			return FVector::ZeroVector;
	}
}
