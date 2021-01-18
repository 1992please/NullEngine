#pragma once
#include "Matrix.h"

class FPerspectiveMatrix
	: public FMatrix
{
public:

	/**
	 * Constructor
	 *
	 * @param HalfFOVX Half FOV in the X axis
	 * @param HalfFOVY Half FOV in the Y axis
	 * @param MultFOVX multiplier on the X axis
	 * @param MultFOVY multiplier on the y axis
	 * @param MinZ distance to the near Z plane
	 * @param MaxZ distance to the far Z plane
	 */
	FPerspectiveMatrix(float HalfFOVX, float HalfFOVY, float MultFOVX, float MultFOVY, float MinZ, float MaxZ);

	/**
	 * Constructor
	 *
	 * @param HalfFOV half Field of View in the Y direction
	 * @param Width view space width
	 * @param Height view space height
	 * @param MinZ distance to the near Z plane
	 * @param MaxZ distance to the far Z plane
	 * @note that the FOV you pass in is actually half the FOV, unlike most perspective matrix functions (D3DXMatrixPerspectiveFovLH).
	 */
	FPerspectiveMatrix(float HalfFOV, float Width, float Height, float MinZ, float MaxZ);
};


FORCEINLINE FPerspectiveMatrix::FPerspectiveMatrix(float HalfFOVX, float HalfFOVY, float MultFOVX, float MultFOVY, float MinZ, float MaxZ)
	: FMatrix(
		FPlane(MultFOVX / FMath::Tan(HalfFOVX), 0.0f, 0.0f, 0.0f),
		FPlane(0.0f, MultFOVY / FMath::Tan(HalfFOVY), 0.0f, 0.0f),
		FPlane(0.0f, 0.0f, (-MinZ - MaxZ) / (MinZ - MaxZ), 1.0f),
		FPlane(0.0f, 0.0f, 2 * MinZ * MaxZ / (MinZ - MaxZ), 0.0f)
	)
{ }


FORCEINLINE FPerspectiveMatrix::FPerspectiveMatrix(float HalfFOV, float Width, float Height, float MinZ, float MaxZ)
	: FMatrix(
		FPlane(Height / (FMath::Tan(HalfFOV) * Width), 0.0f, 0.0f, 0.0f),
		FPlane(0.0f, 1.0F / FMath::Tan(HalfFOV), 0.0f, 0.0f),
		FPlane(0.0f, 0.0f,  (-MinZ - MaxZ)/(MinZ - MaxZ), 1.0f),
		FPlane(0.0f, 0.0f, 2 * MinZ * MaxZ / (MinZ - MaxZ), 0.0f)
	)
{ }
