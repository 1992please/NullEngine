#pragma once
#include "Core/Math/Transform.h"

class FTransformComponent
{
public:
	FTransform Transform;

	FORCEINLINE FVector GetPosition() const { return Transform.GetPosition(); }
	FORCEINLINE void SetPosition(const FVector& InPosition) { Transform.SetPosition(InPosition); }
	FORCEINLINE FQuat GetRotation() const { return Transform.GetRotation(); }
	FORCEINLINE void SetRotation(const FQuat& InQuat) { Transform.SetRotation(InQuat); }
	FORCEINLINE FVector GetScale() const { return Transform.GetScale3D(); }
	FORCEINLINE void SetScale(const FVector& InScale) { Transform.SetScale3D(InScale); }
};