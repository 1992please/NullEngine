#pragma once

#include "Core/CoreTypes.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Plane.h"
#include "Core/Math/Matrix.h"

class FTranslationMatrix
	: public FMatrix
{
public:

	/** Constructor translation matrix based on given vector */
	FORCEINLINE FTranslationMatrix(const FVector& Delta)
		: FMatrix(
			FPlane(1.0f, 0.0f, 0.0f, 0.0f),
			FPlane(0.0f, 1.0f, 0.0f, 0.0f),
			FPlane(0.0f, 0.0f, 1.0f, 0.0f),
			FPlane(Delta.X, Delta.Y, Delta.Z, 1.0f)
		)
	{ }

	/** Matrix factory. Return an FMatrix so we don't have type conversion issues in expressions. */
	static FMatrix Make(FVector const& Delta)
	{
		return FTranslationMatrix(Delta);
	}
};
