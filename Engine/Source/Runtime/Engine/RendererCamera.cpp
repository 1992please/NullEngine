#include "NullPCH.h"
#include "RendererCamera.h"
#include "Core/Math/InvRotationMatrix.h"
#include "Core/Math/TranslationMatrix.h"
#include "Core/Math/RotationMatrix.h"

FOrthographicCamera::FOrthographicCamera(float InWidth, float InHeight)
	: Position(EForceInit::ForceInitToZero)
	, Rotation(EForceInit::ForceInitToZero)
	, ViewMatrix(FMatrix::Identity)
{
	SetProjection(InWidth, InHeight);
}

void FOrthographicCamera::SetProjection(float InWidth, float InHeight)
{
	NE_PROFILE_FUNCTION();

	ProjectionMatrix = FOrthoMatrix(-InWidth * .5f, InWidth * .5f, InHeight * .5f, - InHeight * .5f);
	ViewProjectionMatrix = ViewMatrix * ProjectionMatrix;
}

void FOrthographicCamera::RecalculateViewMatrix()
{
	NE_PROFILE_FUNCTION();

	const FMatrix ViewRotationMatrix = FInverseRotationMatrix(Rotation);

	ViewMatrix = ViewRotationMatrix * FTranslationMatrix(-Position);

	ViewProjectionMatrix = ViewMatrix * ProjectionMatrix;
}