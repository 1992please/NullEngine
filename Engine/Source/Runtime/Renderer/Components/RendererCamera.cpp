#include "RendererCamera.h"
#include "Core/Math/RotationTranslationMatrix.h"

FOrthographicCamera::FOrthographicCamera(float InWidth, float InHeight, float InNearPlane , float InFarPlane)
	: ProjectionMatrix(FReversedZOrthoMatrix(InWidth / 2.0f, InHeight / 2.0f, 1.0f / (InFarPlane - InNearPlane), -InNearPlane))
	, ViewMatrix(FMatrix::Identity)
	, Rotation(0.0f)
{
	ViewProjectionMatrix = ViewMatrix * ProjectionMatrix;
}

void FOrthographicCamera::RecalculateViewMatrix()
{
	FMatrix TransformMat = FRotationTranslationMatrix(FRotator(Rotation, 0.0f, 0.0f), Position);
	ViewMatrix = TransformMat.InverseFast();
	ViewProjectionMatrix = ViewMatrix * ProjectionMatrix;
}
