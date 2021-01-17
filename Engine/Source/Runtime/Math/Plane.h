#pragma once
#include "Vector.h"

struct alignas(16) FPlane 
	: public FVector
{
public:
	float W;
public:


	/** Default constructor (no initialization). */
	FORCEINLINE FPlane();

	/**
	 * Copy Constructor.
	 *
	 * @param P Plane to copy from.
	 */
	FORCEINLINE FPlane(const FPlane& P);

	/**
	 * Constructor.
	 *
	 * @param InX X-coefficient.
	 * @param InY Y-coefficient.
	 * @param InZ Z-coefficient.
	 * @param InW W-coefficient.
	 */
	FORCEINLINE FPlane(float InX, float InY, float InZ, float InW);

	/**
	 * Constructor.
	 *
	 * @param InNormal Plane Normal Vector.
	 * @param InW Plane W-coefficient.
	 */
	FORCEINLINE FPlane(FVector InNormal, float InW);

	/**
	 * Constructor.
	 *
	 * @param InBase Base point in plane.
	 * @param InNormal Plane Normal Vector.
	 */
	FORCEINLINE FPlane(FVector InBase, const FVector &InNormal);

	/**
	 * Constructor.
	 *
	 * @param A First point in the plane.
	 * @param B Second point in the plane.
	 * @param C Third point in the plane.
	 */
	FPlane(FVector A, FVector B, FVector C);

	/**
	 * Constructor
	 *
	 * @param EForceInit Force Init Enum.
	 */
	explicit FORCEINLINE FPlane(EForceInit);
};

FORCEINLINE FPlane::FPlane()
{}


FORCEINLINE FPlane::FPlane(const FPlane& P)
	: FVector(P)
	, W(P.W)
{}

FORCEINLINE FPlane::FPlane(float InX, float InY, float InZ, float InW)
	: FVector(InX, InY, InZ)
	, W(InW)
{}

FORCEINLINE FPlane::FPlane(FVector InNormal, float InW)
	: FVector(InNormal), W(InW)
{}

FORCEINLINE FPlane::FPlane(FVector InBase, const FVector &InNormal)
	: FVector(InNormal)
	, W(InBase | InNormal)
{}

FORCEINLINE FPlane::FPlane(FVector A, FVector B, FVector C)
	: FVector(((B - A) ^ (C - A)).GetSafeNormal())
{
	W = A | (FVector)(*this);
}

FORCEINLINE FPlane::FPlane(EForceInit)
	: FVector(ForceInit), W(0.f)
{}