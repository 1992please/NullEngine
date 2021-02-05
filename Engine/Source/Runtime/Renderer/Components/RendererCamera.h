#pragma once
#include "Core/CoreTypes.h"
#include "Core/Math/OrthoMatrix.h"
#include "Core/Math/Transform.h"
class FOrthographicCamera
{
public:
	FOrthographicCamera(float InWidth, float InHeight, float InNearPlane = 0.0f, float InFarPlane = 1.0f);
	void SetProjection(float InWidth, float InHeight, float InNearPlane = 0.0f, float InFarPlane = 1.0f);

	FORCEINLINE void SetPosition(const FVector& InPostion) { Position = InPostion; RecalculateViewMatrix(); }
	FORCEINLINE void SetRotation(const FRotator& InRotation) { Rotation = InRotation; RecalculateViewMatrix(); }
	FORCEINLINE const FMatrix& GetProjectionMatrix() const { return ProjectionMatrix; }
	FORCEINLINE const FMatrix& GetViewMatrix() const { return ViewMatrix; }
	FORCEINLINE const FMatrix& GetViewProjectionMatrix() const { return ViewProjectionMatrix; }
protected:
	virtual void RecalculateViewMatrix();

	FVector Position;
	FRotator Rotation;
	FMatrix ProjectionMatrix;
	FMatrix ViewMatrix;
	FMatrix ViewProjectionMatrix;
};

class F2DCamera : public FOrthographicCamera
{
public:
	F2DCamera(float InWidth, float InHeight);
	virtual void RecalculateViewMatrix() override;
};