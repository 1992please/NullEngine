#pragma once
#include "Core/CoreTypes.h"
#include "Core/Math/OrthoMatrix.h"

class FOrthographicCamera
{
public:
	FOrthographicCamera(float InWidth, float InHeight, float InNearPlane = 0.0f, float InFarPlane = 1.0f);

	FORCEINLINE void SetPosition(const FVector& InPostion) { Position = InPostion; RecalculateViewMatrix(); }
	FORCEINLINE void SetRotation(const float InRotation) { Rotation = InRotation; RecalculateViewMatrix(); }
	FORCEINLINE const FVector& GetPosition() const { return Position; }
	FORCEINLINE float GetRotation() const { return Rotation; }
	FORCEINLINE const FMatrix& GetProjectionMatrix() const { return ProjectionMatrix; }
	FORCEINLINE const FMatrix& GetViewMatrix() const { return ViewMatrix; }
	FORCEINLINE const FMatrix& GetViewProjectionMatrix() const { return ViewProjectionMatrix; }
private:
	void RecalculateViewMatrix();
private:
	FMatrix ProjectionMatrix;
	FMatrix ViewMatrix;
	FMatrix ViewProjectionMatrix;

	FVector Position;
	float Rotation;
};