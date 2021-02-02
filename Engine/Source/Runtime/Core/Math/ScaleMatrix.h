#pragma once
#include "Core/CoreTypes.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/Plane.h"
class FScaleMatrix
	: public FMatrix
{
public:
	FScaleMatrix( float Scale )
    	: FMatrix(
		FPlane(Scale,	0.0f,	0.0f,	0.0f),
		FPlane(0.0f,	Scale,	0.0f,	0.0f),
		FPlane(0.0f,	0.0f,	Scale,	0.0f),
		FPlane(0.0f,	0.0f,	0.0f,	1.0f)
	    )
    { }

	FScaleMatrix( const FVector& Scale )
    	: FMatrix(
		FPlane(Scale.X,	0.0f,		0.0f,		0.0f),
		FPlane(0.0f,	Scale.Y,	0.0f,		0.0f),
		FPlane(0.0f,	0.0f,		Scale.Z,	0.0f),
		FPlane(0.0f,	0.0f,		0.0f,		1.0f)
        )
    { }
};