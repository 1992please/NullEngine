#pragma once

#include "Core/CoreTypes.h"
#include "Core/Math/Plane.h"
#include "Core/Math/Matrix.h"

class FOrthoMatrix
	: public FMatrix
{
public:
	// Assuming Near is -1 and Far is 1
	FORCEINLINE FOrthoMatrix(float Left, float Right, float Top, float Bottom)
		: FMatrix(
			FPlane(2.0f / (Right - Left), 0.0f, 0.0f, 0.0f),
			FPlane(0.0f, 2.0f / (Top - Bottom), 0.0f, 0.0f),
			FPlane(0.0f, 0.0f, 1.0f, 0.0f),
			FPlane(-(Right + Left) / (Right - Left), -(Top + Bottom) / (Top - Bottom), 0.0f, 1.0f)
		)
	{ }

	// Left Handed system - Map Z to -1 to 1
	FORCEINLINE FOrthoMatrix(float Left, float Right, float Top, float Bottom, float Near, float Far)
		: FMatrix(
			FPlane(2.0f / (Right - Left), 0.0f, 0.0f, 0.0f),
			FPlane(0.0f, 2.0f / (Top - Bottom), 0.0f, 0.0f),
			FPlane(0.0f, 0.0f, 2.0f / (Far - Near), 0.0f),
			FPlane(-(Right + Left) / (Right - Left), -(Top + Bottom) / (Top - Bottom), -(Far + Near) / (Far - Near), 1.0f)
		)
	{ }
};



