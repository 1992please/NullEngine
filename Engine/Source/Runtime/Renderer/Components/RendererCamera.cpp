#include "RendererCamera.h"
#include "Core/Math/InvRotationMatrix.h"
#include "Core/Math/TranslationMatrix.h"
#include "Core/Math/RotationMatrix.h"

FOrthographicCamera::FOrthographicCamera(float InWidth, float InHeight, float InNearPlane , float InFarPlane)
	: ProjectionMatrix(FReversedZOrthoMatrix(InWidth, InHeight, 1.0f / (InFarPlane - InNearPlane), -InNearPlane))
	, Position(EForceInit::ForceInitToZero)
	, Rotation(EForceInit::ForceInitToZero)
	, ViewMatrix(FMatrix::Identity)
{
	ViewProjectionMatrix = ViewMatrix * ProjectionMatrix;
}

void FOrthographicCamera::SetProjection(float InWidth, float InHeight, float InNearPlane /*= 0.0f*/, float InFarPlane /*= 1.0f*/)
{
	ProjectionMatrix = FReversedZOrthoMatrix(InWidth, InHeight, 1.0f / (InFarPlane - InNearPlane), -InNearPlane);
	ViewProjectionMatrix = ViewMatrix * ProjectionMatrix;
}

void FOrthographicCamera::RecalculateViewMatrix()
{
	// Care all calculation hear are inversed matrices 
	// remember the math rule Inverse(AB) = Inverse(B) * Inverse(A)

	// ATTENTION: this is the part of the code where we switch between our coordinate system to rendering api coordinate system
	// swap axis we take values in x we add it in z, y to x, z to y so that z is up
	FMatrix ViewPlanesMatrix = FMatrix(
		FPlane(0, 0, 1, 0),
		FPlane(1, 0, 0, 0),
		FPlane(0, 1, 0, 0),
		FPlane(0, 0, 0, 1));

	const FMatrix ViewRotationMatrix = FInverseRotationMatrix(Rotation);

	ViewMatrix = ViewRotationMatrix * FTranslationMatrix(-Position);

	ViewMatrix = ViewMatrix * ViewPlanesMatrix;

	ViewProjectionMatrix = ViewMatrix * ProjectionMatrix;
}

F2DCamera::F2DCamera(float InWidth, float InHeight)
	: FOrthographicCamera(InWidth, InHeight, -1.0f, 1000.f)
{

}

void F2DCamera::RecalculateViewMatrix()
{
	const FMatrix ViewRotationMatrix = FInverseRotationMatrix(Rotation);

	ViewMatrix = ViewRotationMatrix * FTranslationMatrix(-Position);

	ViewProjectionMatrix = ViewMatrix * ProjectionMatrix;
}
