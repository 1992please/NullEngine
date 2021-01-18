#pragma once
#include "MathUtility.h"

struct FQuat;

struct FRotator
{
public:
	float Pitch;
	float Yaw;
	float Roll;

	static const FRotator ZeroRotator;
	FORCEINLINE FRotator() { }
	explicit FORCEINLINE FRotator(float InF);
	FORCEINLINE FRotator(float InPitch, float InYaw, float InRoll);
	explicit FORCEINLINE FRotator(EForceInit);
	explicit FRotator(const FQuat& Quat);
	FQuat Quaternion() const;
	/**
	 * Clamps an angle to the range of [0, 360).
	 *
	 * @param Angle The angle to clamp.
	 * @return The clamped angle.
	 */
	static float ClampAxis(float Angle);

	/**
	 * Clamps an angle to the range of (-180, 180].
	 *
	 * @param Angle The Angle to clamp.
	 * @return The clamped angle.
	 */
	static float NormalizeAxis(float Angle);
};

FORCEINLINE FRotator::FRotator(float InF)
	: Pitch(InF), Yaw(InF), Roll(InF)
{
}

FORCEINLINE FRotator::FRotator(float InPitch, float InYaw, float InRoll)
	: Pitch(InPitch), Yaw(InYaw), Roll(InRoll)
{
}


FORCEINLINE FRotator::FRotator(EForceInit)
	: Pitch(0), Yaw(0), Roll(0)
{}

FORCEINLINE float FRotator::ClampAxis(float Angle)
{
	// returns Angle in the range (-360,360)
	Angle = FMath::Fmod(Angle, 360.f);

	if (Angle < 0.f)
	{
		// shift to [0,360) range
		Angle += 360.f;
	}

	return Angle;
}


FORCEINLINE float FRotator::NormalizeAxis(float Angle)
{
	// returns Angle in the range [0,360)
	Angle = ClampAxis(Angle);

	if (Angle > 180.f)
	{
		// shift to (-180,180]
		Angle -= 360.f;
	}

	return Angle;
}