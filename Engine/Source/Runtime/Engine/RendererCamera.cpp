#include "NullPCH.h"
#include "RendererCamera.h"
#include "Core/Math/InvRotationMatrix.h"
#include "Core/Math/TranslationMatrix.h"
#include "Core/Math/RotationMatrix.h"

FOrthographicCamera::FOrthographicCamera(float InWidth, float InHeight, float InNearPlane , float InFarPlane)
	: Position(EForceInit::ForceInitToZero)
	, Rotation(EForceInit::ForceInitToZero)
	, ViewMatrix(FMatrix::Identity)
{
	SetProjection(InWidth, InHeight, InNearPlane, InFarPlane);
}

void FOrthographicCamera::SetProjection(float InWidth, float InHeight, float InNearPlane /*= 0.0f*/, float InFarPlane /*= 1.0f*/)
{
	NE_PROFILE_FUNCTION();

	ProjectionMatrix = FReversedZOrthoMatrix(InWidth, InHeight, 2.0f / (InFarPlane - InNearPlane), -InNearPlane);
	FVector Result1 = ProjectionMatrix.TransformPosition(FVector(-0.1f, -0.1f, 0.5f));
	FVector Result2 = ProjectionMatrix.TransformPosition(FVector(-0.1f, -0.1f, 0.0f));
	FVector Result3 = ProjectionMatrix.TransformPosition(FVector(-0.1f, -0.1f, .99f));
	ViewProjectionMatrix = ViewMatrix * ProjectionMatrix;
}

void FOrthographicCamera::RecalculateViewMatrix()
{
	NE_PROFILE_FUNCTION();
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

void F2DCamera::RecalculateViewMatrix()
{
	NE_PROFILE_FUNCTION();

	const FMatrix ViewRotationMatrix = FInverseRotationMatrix(Rotation);

	ViewMatrix = ViewRotationMatrix * FTranslationMatrix(-Position);

	ViewProjectionMatrix = ViewMatrix * ProjectionMatrix;
}
