#include "NullPCH.h"
#include "Rotator.h"
#include "Quat.h"

const FRotator FRotator::ZeroRotator(0.f, 0.f, 0.f);

FRotator::FRotator(const FQuat& Quat)
{
	*this = Quat.Rotator();
}

FQuat FRotator::Quaternion() const
{
	const float DEG_TO_RAD = PI / (180.f);
	const float RADS_DIVIDED_BY_2 = DEG_TO_RAD * .5f;
	float SP, SY, SR;
	float CP, CY, CR;

	const float PitchNoWinding = FMath::Fmod(Pitch, 360.0f);
	const float YawNoWinding = FMath::Fmod(Yaw, 360.0f);
	const float RollNoWinding = FMath::Fmod(Roll, 360.0f);


	FMath::SinCos(&SP, &CP, PitchNoWinding * RADS_DIVIDED_BY_2);
	FMath::SinCos(&SY, &CY, YawNoWinding * RADS_DIVIDED_BY_2);
	FMath::SinCos(&SR, &CR, RollNoWinding * RADS_DIVIDED_BY_2);

	FQuat RotationQuat;
	RotationQuat.X = -CR * SP*CY - SR * CP*SY;
	RotationQuat.Y =  CR * CP*SY - SR * SP*CY;
	RotationQuat.Z =  CR * SP*SY - SR * CP*CY;
	RotationQuat.W = CR * CP*CY + SR * SP*SY;
	return RotationQuat;
}
