#pragma once
#include "Core/CoreTypes.h"
#include "Core/Math/MathUtility.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/Plane.h"


/** Inverse Rotation matrix */
class FInverseRotationMatrix
	: public FMatrix
{
public:
	/**
	 * Constructor.
	 *
	 * @param Rot rotation
	 */
	FInverseRotationMatrix(const FRotator& Rot);
};


FORCEINLINE FInverseRotationMatrix::FInverseRotationMatrix(const FRotator& Rot)
	: FMatrix(
		FMatrix( // Yaw
		FPlane(+FMath::Cos(Rot.Yaw * PI / 180.f), 0.0f, +FMath::Sin(Rot.Yaw * PI / 180.f), 0.0f),
		FPlane(0.0f, 1.0f, 0.0f, 0.0f),
		FPlane(-FMath::Sin(Rot.Yaw * PI / 180.f), 0.0f, +FMath::Cos(Rot.Yaw * PI / 180.f), 0.0f),
		FPlane(0.0f, 0.0f, 0.0f, 1.0f)) *
		FMatrix( // Pitch
		FPlane(1.0f, 0.0f, 0.0f, 0.0f),
		FPlane(0.0f, +FMath::Cos(Rot.Pitch * PI / 180.f), +FMath::Sin(Rot.Pitch * PI / 180.f), 0.0f),
		FPlane(0.0f, -FMath::Sin(Rot.Pitch * PI / 180.f), +FMath::Cos(Rot.Pitch * PI / 180.f), 0.0f),
		FPlane(0.0f, 0.0f, 0.0f, 1.0f)) *
		FMatrix( // Roll
		FPlane(+FMath::Cos(Rot.Roll * PI / 180.f), +FMath::Sin(Rot.Roll * PI / 180.f), 0.0f, 0.0f),
		FPlane(-FMath::Sin(Rot.Roll * PI / 180.f), +FMath::Cos(Rot.Roll * PI / 180.f), 0.0f, 0.0f),
		FPlane(0.0f, 0.0f, 1.0f, 0.0f),
		FPlane(0.0f, 0.0f, 0.0f, 1.0f)) 
	)
{ }
