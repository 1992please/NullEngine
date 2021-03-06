#pragma once
#include "MathUtility.h"
#include "Core/Containers/String.h"

struct FQuat;
/*
* Pitch: Rotation around the right axis (around Y axis), Looking up and down (0=Straight Ahead, +Up, -Down) ---> not left handed rotation
* Yaw: Rotation around the up axis (around Z axis), Running in circles 0=East, +North, -South. 
* Roll: Rotation around the forward axis (around X axis), Tilting your head, 0=Straight, +Clockwise, -CCW.  ---> not left handed rotation
* not left handed rotations will be always negative in the calculations
*/
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

	static float ClampAxis(float Angle);

	static float NormalizeAxis(float Angle);

	FORCEINLINE FString FRotator::ToString() const
	{
		return FString::Printf("P=%f Y=%f R=%f", Pitch, Yaw, Roll);
	}
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