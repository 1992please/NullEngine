#include "Transform.h"

const FTransform FTransform::Identity(FQuat(0.f, 0.f, 0.f, 1.f), FVector(0.f), FVector(1.f));

FString FTransform::ToString() const
{
	const FRotator R(Rotator());
	const FVector T(GetTranslation());
	const FVector S(GetScale3D());

	return FString::Printf(TEXT("%f,%f,%f|%f,%f,%f|%f,%f,%f"), T.X, T.Y, T.Z, R.Pitch, R.Yaw, R.Roll, S.X, S.Y, S.Z);
}
