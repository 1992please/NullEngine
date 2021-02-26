#pragma once
#include "Matrix.h"

class FPerspectiveMatrix
	: public FMatrix
{
public:

	// Far is infinite
	FORCEINLINE FPerspectiveMatrix(float Fovy, float Aspect, float Near)
	{ 
		const float Range = FMath::Tan(Fovy / 2.0f) * Near;
		const float Left = -Range * Aspect;
		const float Right = Range * Aspect;
		const float Bottom = -Range;
		const float Top = Range;

		FMemory::Memzero(this, sizeof(*this));

		M[0][0] = (2.0f * Near) / (Right - Left);
		M[1][1] = (2.0f * Near) / (Top - Bottom);
		M[2][2] = 1.f; 
		M[2][3] = 1.f;
		M[3][2] = -2 * Near;
	}

	FORCEINLINE FPerspectiveMatrix::FPerspectiveMatrix(float Fovy, float Aspect, float Near, float Far)
		: FMatrix(
			FPlane(1.0f / (FMath::Tan(Fovy / 2.0f) * Aspect), 0.0f, 0.0f, 0.0f),
			FPlane(0.0f, 1.0f / FMath::Tan(Fovy / 2.0f), 0.0f, 0.0f),
			FPlane(0.0f, 0.0f, (Far + Near) / (Far - Near), 1.0f),
			FPlane(0.0f, 0.0f, -2.0f * Near * Far / (Far - Near), 0.0f)
		)
	{ }
};






