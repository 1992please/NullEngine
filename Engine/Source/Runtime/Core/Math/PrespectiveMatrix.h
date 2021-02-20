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


		FMatrix(
			FPlane((2.0f * Near) / (Right - Left), 0.0f, 0.0f, 0.0f),
			FPlane(0.0f, (2.0f * Near) / (Top - Bottom), 0.0f, 0.0f),
			FPlane(0.0f, 0.0f, 1.0f, 1.0f),
			FPlane(0.0f, 0.0f, -2 * Near, 0.0f)
		);
	}

	FORCEINLINE FPerspectiveMatrix::FPerspectiveMatrix(float Fovy, float Aspect, float Near, float Far)
	{ 
		const float TanHalfFovy = FMath::Tan(Fovy / 2.0f);

		FMatrix(
			FPlane(1.0f / (TanHalfFovy * Aspect), 0.0f, 0.0f, 0.0f),
			FPlane(0.0f, 1.0f / TanHalfFovy, 0.0f, 0.0f),
			FPlane(0.0f, 0.0f, (Far + Near) / (Far - Near), 1.0f),
			FPlane(0.0f, 0.0f, -2.0f * Near * Far / (Far - Near), 0.0f)
		);
	
	}
};






