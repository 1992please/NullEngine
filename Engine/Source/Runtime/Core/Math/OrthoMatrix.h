#pragma once

#include "Core/CoreTypes.h"
#include "Core/Math/Plane.h"
#include "Core/Math/Matrix.h"

class FOrthoMatrix
	: public FMatrix
{
public:
	FORCEINLINE FOrthoMatrix(float Width,float Height,float ZScale,float ZOffset)
	: FMatrix(
		FPlane((Width != 0.0f) ? (1.0f / Width) : 1.0f, 0.0f, 0.0f, 0.0f),
		FPlane(0.0f, (Height != 0.0f) ? (1.0f / Height) : 1.f, 0.0f, 0.0f),
		FPlane(0.0f, 0.0f, ZScale, 0.0f),
		FPlane(0.0f, 0.0f, ZOffset * ZScale, 1.0f)
	)
{ }
};

class FReversedZOrthoMatrix : public FMatrix
{
public:
	FORCEINLINE FReversedZOrthoMatrix(float Width,float Height,float ZScale,float ZOffset)
	: FMatrix(
		FPlane((Width != 0.0f) ? (1.0f / Width) : 1.0f, 0.0f, 0.0f, 0.0f),
		FPlane(0.0f, (Height != 0.0f) ? (1.0f / Height) : 1.f, 0.0f, 0.0f),
		FPlane(0.0f, 0.0f, -ZScale, 0.0f),
		FPlane(0.0f, 0.0f, 1.0f - ZOffset * ZScale, 1.0f)
	)
{ }
};


